#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "IView.h"

// ScreenManager — owns the big OLED and decides which view is currently
// painting on it. Views (Face, Weather, News, etc.) are guests; this
// class is the host.
//
// Behavior right now: AUTO_RETURN
//   - There is a "default" view (typically the face) that is always shown
//     unless explicitly overridden.
//   - When you call ShowView(view, durationMs), that view takes over for
//     `durationMs` milliseconds. After the timer expires, it auto-returns
//     to the default view.
//   - Other modes (STAY_ON_SELECTED, ROTATE) can be added later without
//     changing this class's public API.
class ScreenManager {
  public:
    // Constructor with sensible defaults — works as-is for the current
    // big OLED but configurable for future displays.
    ScreenManager(int width  = 128,
                  int height = 64,
                  uint8_t i2cAddress = 0x3C,
                  int sdaPin = 8,
                  int sclPin = 9,
                  TwoWire* wire = &Wire);

    // Hardware setup. Call once in setup(). Returns false if the OLED
    // wasn't found at the configured address.
    bool Init();

    // Call every iteration of loop(). Handles:
    //   - auto-return timer
    //   - asking the current view to draw
    void Update();

    // Configures which view is the always-on background.
    // Typical use: SetDefaultView(&face)
    void SetDefaultView(IView* view);

    // Temporarily switches the screen to `view` for `durationMs` ms.
    // After the timer expires, returns to the default view.
    // OnEnter()/OnExit() are called appropriately on the views.
    void ShowView(IView* view, unsigned long durationMs);

    // Force a return to the default view immediately.
    void ReturnToDefault();

    // Read access to the underlying display, in case some view needs
    // to do something exotic. Most code should not use this.
    Adafruit_SSD1306& GetDisplay() { return _display; }

  private:
    // Hardware — owned by this class
    Adafruit_SSD1306 _display;
    uint8_t _i2cAddress;
    int _sdaPin;
    int _sclPin;
    TwoWire* _wire;

    // View bookkeeping
    IView* _defaultView;       // the always-on view, never null after setup
    IView* _currentView;       // either == _defaultView or a takeover view
    unsigned long _viewStartedAt;
    unsigned long _viewDurationMs;

    // Internal helper: switch active view, calling OnExit/OnEnter
    void _switchTo(IView* view);
};
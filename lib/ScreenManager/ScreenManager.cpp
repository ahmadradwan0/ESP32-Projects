#include "ScreenManager.h"

#define OLED_RESET_PIN -1   // SSD1306 needs this; -1 = "no reset pin used"

ScreenManager::ScreenManager(int width, int height, uint8_t i2cAddress,
                             int sdaPin, int sclPin, TwoWire* wire)
  : _display(width, height, wire, OLED_RESET_PIN),
    _i2cAddress(i2cAddress),
    _sdaPin(sdaPin),
    _sclPin(sclPin),
    _wire(wire),
    _defaultView(nullptr),
    _currentView(nullptr),
    _viewStartedAt(0),
    _viewDurationMs(0)
{
}

bool ScreenManager::Init() {
  _wire->begin(_sdaPin, _sclPin);

  if (!_display.begin(SSD1306_SWITCHCAPVCC, _i2cAddress)) {
    return false;
  }

  // Start with a clean black screen
  _display.clearDisplay();
  _display.display();
  return true;
}

void ScreenManager::SetDefaultView(IView* view) {
  _defaultView = view;

  // If no view was active yet, make this one active and let it know
  if (_currentView == nullptr) {
    _switchTo(view);
  }
}

void ScreenManager::ShowView(IView* view, unsigned long durationMs) {
  if (view == nullptr) return;

  _switchTo(view);
  _viewStartedAt  = millis();
  _viewDurationMs = durationMs;
}

void ScreenManager::ReturnToDefault() {
  _switchTo(_defaultView);
  _viewDurationMs = 0;   // 0 means "no auto-return needed"
}

void ScreenManager::Update() {
  // Check auto-return timer
  if (_currentView != _defaultView && _viewDurationMs > 0) {
    if (millis() - _viewStartedAt >= _viewDurationMs) {
      _switchTo(_defaultView);
      _viewDurationMs = 0;
    }
  }

  // Draw whatever the current view is
  if (_currentView != nullptr) {
    _currentView->Draw(_display);
  }
}

void ScreenManager::_switchTo(IView* view) {
  if (view == _currentView) return;   // no change, skip lifecycle calls

  // Tell the outgoing view it's no longer active
  if (_currentView != nullptr) {
    _currentView->OnExit();
  }

  _currentView = view;

  // Tell the incoming view it just became active
  if (_currentView != nullptr) {
    _currentView->OnEnter();
  }
}
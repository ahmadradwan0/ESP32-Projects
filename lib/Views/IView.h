#pragma once
#include <Adafruit_SSD1306.h>

class IView {
  public:
    // virtual destructor — required for base classes
    virtual ~IView() {}

    // Called when this view becomes the active one
    virtual void OnEnter() {}

    // Called when this view stops being active
    virtual void OnExit() {}

    // Called every loop while this view is active.
    // Receives the display so the view can draw whatever it wants.
    virtual void Draw(Adafruit_SSD1306& display) = 0;
};
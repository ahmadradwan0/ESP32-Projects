#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "IView.h"
#include "ISound.h"

class ClockView : public IView {
  public:
    explicit ClockView(ISound& sound);

    void OnEnter() override;
    void Draw(Adafruit_SSD1306& display) override;

  private:
    ISound& _sound;
};
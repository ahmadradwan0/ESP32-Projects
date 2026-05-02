#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "IView.h"
#include "ISound.h"

class WeatherView : public IView {
  public:
    explicit WeatherView(ISound& sound);

    void OnEnter() override;
    void OnExit() override;
    void Draw(Adafruit_SSD1306& display) override;

  private:
    ISound& _sound;

    bool    _dataReady;
    float   _tempF;
    float   _tempC;
    String  _conditionText;
    String  _errorMsg;

    bool _fetchWeather();
    String _weatherCodeToString(int code);
};
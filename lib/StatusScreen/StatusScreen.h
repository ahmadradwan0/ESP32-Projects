#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class StatusScreen {
  public:
    StatusScreen();
    bool Init();
    void Clear();
    void Show();
    void ShowText(String msg, int x, int y, int size);
    void ShowMessage(String line1, String line2 = "");
    void ShowRequest(String label);

  private:
    TwoWire _bus;                 // our own I2C bus instance
    Adafruit_SSD1306 _display;
    bool _ready;
};
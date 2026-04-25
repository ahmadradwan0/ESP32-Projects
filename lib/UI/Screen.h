#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Screen
{
    public:
        static const uint16_t COLOR_ON = SSD1306_WHITE;
        static const uint16_t COLOR_OFF = SSD1306_BLACK;

        Screen();
        bool init();
        void clear();
        void showText(String msg, int size = 2);
        void showTwoLine(String line1, String line2);

    private:
        Adafruit_SSD1306 _display;
};
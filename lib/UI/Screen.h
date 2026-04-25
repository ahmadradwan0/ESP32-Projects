#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Screen
{
    public:
        static const uint16_t COLOR_ON = SSD1306_WHITE;
        static const uint16_t COLOR_OFF = SSD1306_BLACK;

        Screen();
        bool Init();
        void Clear();
        void Screen::ShowText(String msg,int x, int y ,int size);
        void ShowTwoLine(String line1, String line2);
        void Screen::Show();
        int Screen::GetTextPixelWidth(String text, int size);

    private:
        Adafruit_SSD1306 _display;
};
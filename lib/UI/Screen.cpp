#include <Screen.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Screen::Screen() : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

bool Screen::Init()
{
    Wire.begin(8, 9);

    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    {
        return false;
    }

    _display.setTextColor(COLOR_ON);
    _display.setTextWrap(false);
    Clear();
    return true;
}

void Screen::Clear()
{
    _display.clearDisplay();
}

void Screen::ShowText(String msg, int x, int y, int size)
{
    _display.setTextSize(size);
    _display.setCursor(x, y);
    _display.print(msg);
}

void Screen::ShowTwoLine(String line1, String line2)
{
    _display.setTextSize(1);
    _display.setCursor(0, 0);
    _display.println(line1);
    _display.setTextSize(2);
    _display.setCursor(0, 16);
    _display.println(line2);
}

void Screen::Show()
{
    _display.display();
}

int Screen::GetTextPixelWidth(String text, int size)
{
    return text.length() * 6 * size;
}
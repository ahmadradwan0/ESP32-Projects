#include <Screen.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Screen::Screen() : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{

}

bool Screen::init()
{
    Wire.begin(8, 9);

    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }

    _display.setTextColor(COLOR_ON);
    _display.setTextWrap(true);
    clear();
    return true;
}

void Screen::clear()
{
    _display.clearDisplay();
    _display.display();
}

void Screen::showText(String msg, int size)
{
    _display.clearDisplay();
    _display.setTextSize(size);
    _display.setCursor(0, 0);
    _display.println(msg);
    _display.display();
}

void Screen::showTwoLine(String line1, String line2) {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setCursor(0, 0);
    _display.println(line1);
    _display.setTextSize(2);
    _display.setCursor(0, 16);
    _display.println(line2);
    _display.display();
}
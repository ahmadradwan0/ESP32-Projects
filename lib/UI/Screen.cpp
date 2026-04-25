#include <Screen.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Screen::Screen() : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

// Returns true if the display was successfully initialized, false otherwise
bool Screen::Init()
{
    Wire.begin(8, 9);

    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }

    _display.setTextColor(COLOR_ON);
    _display.setTextWrap(false);
    _display.clearDisplay();
    _display.display();
    return true;
}

// Clears the display buffer (does not update the screen until Show() is called)    
void Screen::Clear()
{
    _display.clearDisplay();
}

// Displays the given text at the specified position and size (size is a multiplier, e.g. 2 = double size)
void Screen::ShowText(String msg, int x, int y, int size)
{
    _display.setTextSize(size);
    _display.setCursor(x, y);
    _display.print(msg);
}

// Displays two DrawDividers
void Screen::DrawDividers() 
{
    _display.drawLine(0, 8, 127, 0, COLOR_ON);     // top edge
    _display.drawLine(0, 63, 127, 63, COLOR_ON);   // bottom edge
}

// Updates the physical display with the current contents of the display buffer
void Screen::Show()
{
    _display.display();
}

// Calculates the pixel width of a given text string at a specified size (size is a multiplier, e.g. 2 = double size)
int Screen::GetTextPixelWidth(String text, int size)
{
    return text.length() * 6 * size;
}
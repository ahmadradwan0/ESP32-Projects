#include "StatusScreen.h"

#define SDA_PIN 17
#define SCL_PIN 18
#define OLED_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 64

StatusScreen::StatusScreen()
  : _bus(1),                              // I2C controller 1 (second bus)
    _display(WIDTH, HEIGHT, &_bus, -1),   // tell SSD1306 to use our bus
    _ready(false)
{
}

bool StatusScreen::Init() {
  _bus.begin(SDA_PIN, SCL_PIN);

  if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    return false;
  }

  _display.setTextColor(SSD1306_WHITE);
  _display.setTextWrap(false);
  _display.clearDisplay();
  _display.display();
  _ready = true;
  return true;
}

void StatusScreen::Clear() {
  if (!_ready) return;
  _display.clearDisplay();
}

void StatusScreen::Show() {
  if (!_ready) return;
  _display.display();
}

void StatusScreen::ShowText(String msg, int x, int y, int size) {
  if (!_ready) return;
  _display.setTextSize(size);
  _display.setCursor(x, y);
  _display.print(msg);
}

void StatusScreen::ShowMessage(String line1, String line2) {
  if (!_ready) return;
  _display.clearDisplay();
  _display.setTextSize(1);
  _display.setCursor(0, 0);
  _display.print(line1);
  if (line2.length() > 0) {
    _display.setTextSize(2);
    _display.setCursor(0, 14);
    _display.print(line2);
  }
  _display.display();
}

void StatusScreen::ShowRequest(String label) {
  ShowMessage("Request:", label);
}
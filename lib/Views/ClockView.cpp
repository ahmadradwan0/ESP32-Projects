#include "ClockView.h"
#include <time.h>

ClockView::ClockView(ISound& sound)
  : _sound(sound)
{
}

void ClockView::OnEnter() {
  _sound.ClockChimes();
}

void ClockView::Draw(Adafruit_SSD1306& display) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print("No Time");
    display.display();
    return;
  }

  // Hour and minute in 12-hour format
  char timeStr[9];
  int hr = timeinfo.tm_hour % 12;
  if (hr == 0) hr = 12;
  snprintf(timeStr, 9, "%2d:%02d", hr, timeinfo.tm_min);

  // AM/PM
  const char* ampm = timeinfo.tm_hour < 12 ? "AM" : "PM";

  // Date (e.g., "Sat May 2")
  char dateStr[20];
  strftime(dateStr, sizeof(dateStr), "%a %b %d", &timeinfo);

  // --- Draw ---
  // Time – big
  display.setTextSize(3);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  int x = (128 - w) / 2;
  display.setCursor(x, 8);
  display.print(timeStr);

  // AM/PM – small, next to time
  display.setTextSize(1);
  display.setCursor(x + w + 4, 20);
  display.print(ampm);

  // Date – below
  display.setTextSize(1);
  display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
  x = (128 - w) / 2;
  display.setCursor(x, 48);
  display.print(dateStr);

  display.display();
}
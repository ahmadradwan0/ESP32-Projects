#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String message = "Hello world! ESP32-S3 is awesome!";
int textX = 128;        // start position: just off the right edge
int textSize = 3;
int textPixelWidth;     // how wide the full message is in pixels

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);   // ⚠️ critical: don't wrap to next line
  
  // calculate how wide the text is so we know when it's fully off-screen
  textPixelWidth = message.length() * 6 * textSize;
}

void loop() {
  display.clearDisplay();
  
  display.setTextSize(textSize);
  display.setCursor(textX, 20);
  display.print(message);
  
  display.display();
  
  textX -= 2;   // move 2 pixels to the left each frame
  
  // if the text has scrolled completely off the left side, reset to the right
  if (textX < -textPixelWidth) {
    textX = 128;
  }
  
  delay(30);    // smaller delay = faster scroll
}
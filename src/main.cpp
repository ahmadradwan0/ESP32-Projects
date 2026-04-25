#include <Arduino.h>
#include <Screen.h>
#include <TextScroller.h>

Screen screen;
TextScroller scroller(&screen);


void setup() {
  Serial.begin(115200);

  if (!screen.Init()) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  
  scroller.SetMessage("Hello world! ESP32-S3 is awesome!");
  scroller.SetSize(3);
  scroller.SetSpeed(2);
  scroller.SetY(30);
}

void loop() {
  scroller.Update();
  delay(30);
}
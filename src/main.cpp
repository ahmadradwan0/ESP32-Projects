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
  
  scroller.SetMessage("This Message Scrolls Across The Screen!");
  scroller.SetSize(2);
  scroller.SetSpeed(3);
  scroller.SetY(30);
}

void loop() {
  scroller.Update();
  screen.DrawDividers();
  screen.Show();
  delay(30);
}
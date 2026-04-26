#include <Arduino.h>
#include <Face.h>

Face face;

void setup() {
  if (!face.Init()) for(;;);
  face.AutonomousMode(true);
}

void loop() {
  face.Update();
}
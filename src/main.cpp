#include <Arduino.h>
#include <Face.h>

#define BUTTON_PIN 4

Face face;

// Button debouncing state
bool _lastButtonState = HIGH;
unsigned long _lastDebounceTime = 0;
const unsigned long DEBOUNCE_MS = 50;

void setup() {
  if (!face.Init()) for(;;);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  face.EnableSound(false);     // start silent
  face.AutonomousMode(true);   // face still moves on its own, just silently
}

void loop() {
  face.Update();

  // Check button (with debounce)
  bool currentState = digitalRead(BUTTON_PIN);

  if (currentState != _lastButtonState) {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > DEBOUNCE_MS) {
    static bool stableState = HIGH;
    if (currentState != stableState) {
      stableState = currentState;
      if (stableState == LOW) {
        // Button was just pressed
        face.TriggerRandomEmotion();
      }
    }
  }

  _lastButtonState = currentState;
}
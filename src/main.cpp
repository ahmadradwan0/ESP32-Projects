#include <Arduino.h>
#include <Face.h>
#include <AppServer.h>
#include "../html/control_panel.h"
#include <Wire.h> 

// ============================================================
//  GLOBALS
// ============================================================

Face face;
AppServer server;

const char* WIFI_SSID     = "IoT";
const char* WIFI_PASSWORD = "Qwerty@384#";

#define BUTTON_PIN 4
const unsigned long DEBOUNCE_MS = 50;

bool _lastButtonState = HIGH;
unsigned long _lastDebounceTime = 0;

// ============================================================
//  ROUTE HANDLERS
// ============================================================

void registerHomePage() {
  server.OnGetHTML("/", []() {
    return String(CONTROL_PANEL_HTML);
  });
}

void registerMoodRoutes() {
  server.OnGetText("/face/happy",   []() { face.Happy();   return "ok"; });
  server.OnGetText("/face/angry",   []() { face.Angry();   return "ok"; });
  server.OnGetText("/face/tired",   []() { face.Tired();   return "ok"; });
  server.OnGetText("/face/neutral", []() { face.Neutral(); return "ok"; });
}

void registerLookRoutes() {
  server.OnGetText("/look/up",     []() { face.LookUp();     return "ok"; });
  server.OnGetText("/look/down",   []() { face.LookDown();   return "ok"; });
  server.OnGetText("/look/left",   []() { face.LookLeft();   return "ok"; });
  server.OnGetText("/look/right",  []() { face.LookRight();  return "ok"; });
  server.OnGetText("/look/center", []() { face.LookCenter(); return "ok"; });
}

void registerActionRoutes() {
  server.OnGetText("/face/blink",     []() { face.Blink();     return "ok"; });
  server.OnGetText("/face/winkleft",  []() { face.WinkLeft();  return "ok"; });
  server.OnGetText("/face/winkright", []() { face.WinkRight(); return "ok"; });
  server.OnGetText("/face/laugh",     []() { face.Laugh();     return "ok"; });
  server.OnGetText("/face/confused",  []() { face.Confused();  return "ok"; });
}

void registerStatusRoute() {
  server.OnGetJSON("/status", []() {
    return String("{\"alive\":true}");
  });
}

// ============================================================
//  INITIALIZATION
// ============================================================

void initFace() {
  if (!face.Init()) {
    for (;;);
  }
  face.AutonomousMode(true);
  face.EnableSound(false);
}

void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void initServer() {
  server.SetWifi(WIFI_SSID, WIFI_PASSWORD);

  server.OnConnected([](IPAddress ip) {
    // Hook for when WiFi connects
  });

  registerHomePage();
  registerMoodRoutes();
  registerLookRoutes();
  registerActionRoutes();
  registerStatusRoute();

  server.Start();
}

// ============================================================
//  RUNTIME LOOP HELPERS
// ============================================================

void handleButton() {
  bool currentState = digitalRead(BUTTON_PIN);

  if (currentState != _lastButtonState) {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > DEBOUNCE_MS) {
    static bool stableState = HIGH;
    if (currentState != stableState) {
      stableState = currentState;
      if (stableState == LOW) {
        face.TriggerRandomEmotion();
      }
    }
  }

  _lastButtonState = currentState;
}

// ============================================================
//  ARDUINO ENTRY POINTS
// ============================================================

void setup() {
  initFace();
  initButton();
  initServer();
}

void loop() {
  face.Update();
  server.Update();
  handleButton();
}
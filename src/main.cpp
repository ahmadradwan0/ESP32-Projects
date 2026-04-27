#include <Arduino.h>
#include <Face.h>
#include <AppServer.h>
#include <StatusScreen.h>
#include "../html/control_panel.h"
#include <Wire.h>

// ============================================================
//  GLOBALS
// ============================================================

Face face;
AppServer server;
StatusScreen statusScreen;

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
    statusScreen.ShowRequest("HOME");
    return String(CONTROL_PANEL_HTML);
  });
}

void registerMoodRoutes() {
  server.OnGetText("/face/happy",   []() { statusScreen.ShowRequest("HAPPY");   face.Happy();   return "ok"; });
  server.OnGetText("/face/angry",   []() { statusScreen.ShowRequest("ANGRY");   face.Angry();   return "ok"; });
  server.OnGetText("/face/tired",   []() { statusScreen.ShowRequest("TIRED");   face.Tired();   return "ok"; });
  server.OnGetText("/face/neutral", []() { statusScreen.ShowRequest("NEUTRAL"); face.Neutral(); return "ok"; });
}

void registerLookRoutes() {
  server.OnGetText("/look/up",     []() { statusScreen.ShowRequest("UP");     face.LookUp();     return "ok"; });
  server.OnGetText("/look/down",   []() { statusScreen.ShowRequest("DOWN");   face.LookDown();   return "ok"; });
  server.OnGetText("/look/left",   []() { statusScreen.ShowRequest("LEFT");   face.LookLeft();   return "ok"; });
  server.OnGetText("/look/right",  []() { statusScreen.ShowRequest("RIGHT");  face.LookRight();  return "ok"; });
  server.OnGetText("/look/center", []() { statusScreen.ShowRequest("CENTER"); face.LookCenter(); return "ok"; });
}

void registerActionRoutes() {
  server.OnGetText("/face/blink",     []() { statusScreen.ShowRequest("BLINK");    face.Blink();     return "ok"; });
  server.OnGetText("/face/winkleft",  []() { statusScreen.ShowRequest("WINK L");   face.WinkLeft();  return "ok"; });
  server.OnGetText("/face/winkright", []() { statusScreen.ShowRequest("WINK R");   face.WinkRight(); return "ok"; });
  server.OnGetText("/face/laugh",     []() { statusScreen.ShowRequest("LAUGH");    face.Laugh();     return "ok"; });
  server.OnGetText("/face/confused",  []() { statusScreen.ShowRequest("CONFUSED"); face.Confused();  return "ok"; });
}

void registerStatusRoute() {
  server.OnGetJSON("/status", []() {
    statusScreen.ShowRequest("STATUS");
    return String("{\"alive\":true}");
  });
}

// ============================================================
//  INITIALIZATION
// ============================================================

void initStatusScreen() {
  if (statusScreen.Init()) {
    statusScreen.ShowMessage("Booting...", "");
  }
}

void initFace() {
  if (!face.Init()) {
    statusScreen.ShowMessage("Face init", "FAILED");
    for (;;);
  }
  face.AutonomousMode(true);
  face.EnableSound(false);
}

void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void initServer() {
  statusScreen.ShowMessage("WiFi:", "Connecting...");

  server.SetWifi(WIFI_SSID, WIFI_PASSWORD);

  server.OnConnected([](IPAddress ip) {
    statusScreen.ShowMessage("IP:", ip.toString());
  });

  registerHomePage();
  registerMoodRoutes();
  registerLookRoutes();
  registerActionRoutes();
  registerStatusRoute();

  if (!server.Start()) {
    statusScreen.ShowMessage("WiFi", "FAILED");
  }
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
        statusScreen.ShowRequest("BUTTON");
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
  initStatusScreen();   // first — so other init can show messages
  initFace();
  initButton();
  initServer();
}

void loop() {
  face.Update();
  server.Update();
  handleButton();
}
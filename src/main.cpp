#include <Arduino.h>
#include <AppServer.h>
#include <StatusScreen.h>
#include <ScreenManager.h>
#include <FaceView.h>
#include <BuzzerSound.h>
#include "../html/control_panel.h"

// ============================================================
//  GLOBALS
// ============================================================

// Sound: a single buzzer is shared across every view (DI target).
BuzzerSound  buzzer(5);

// FaceView: takes ISound& by reference at construction. The display is
// bound separately, after ScreenManager has begun its OLED.
FaceView     face(buzzer);

// ScreenManager: owns the big OLED. Defaults match the old Face setup
// (128x64, addr 0x3C, SDA=8, SCL=9, &Wire).
ScreenManager screenManager;

// Independent of the view system — small OLED on its own I2C bus.
StatusScreen statusScreen;

AppServer    server;

const char* WIFI_SSID     = "IoT";
const char* WIFI_PASSWORD = "Qwerty@384#";

#define BUTTON_PIN 4
const unsigned long DEBOUNCE_MS = 50;

bool _lastButtonState = HIGH;
unsigned long _lastDebounceTime = 0;

// ============================================================
//  ROUTE HANDLERS — unchanged: face.* surface is identical
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
  // 1. ScreenManager owns and inits the big OLED (Wire.begin(8, 9) etc.)
  if (!screenManager.Init()) {
    statusScreen.ShowMessage("Screen init", "FAILED");
    for (;;);
  }

  // 2. FaceView binds to that display — RoboEyes is allocated here.
  face.Bind(screenManager.GetDisplay());

  // 3. Sound is shared infrastructure now — init it once in main.
  buzzer.Init();

  // 4. Tell ScreenManager the face is the always-on default view.
  screenManager.SetDefaultView(&face);

  // 5. Same as before: alive but quiet ambient mood.
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
  initStatusScreen();
  initFace();
  initButton();
  initServer();
}

void loop() {
  buzzer.Update();           // advance sound queue (was inside face.Update)
  screenManager.Update();    // calls face.Draw() under the hood
  server.Update();
  handleButton();
}
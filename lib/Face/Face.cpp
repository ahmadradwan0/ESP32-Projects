#include "Face.h"
#include <Wire.h>
#include <FluxGarage_RoboEyes.h>   // ← only included here

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define FRAMERATE 50

Face::Face() 
  : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1),
    _eyes(nullptr)
{
}

Face::~Face() {
  delete _eyes;
}

bool Face::Init() {
  Wire.begin(8, 9);

  if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    return false;
  }

  _eyes = new RoboEyes<Adafruit_SSD1306>(_display);
  _eyes->begin(SCREEN_WIDTH, SCREEN_HEIGHT, FRAMERATE);
  _eyes->setAutoblinker(ON, 3, 2);
  _eyes->setIdleMode(ON, 2, 2);
  return true;
}

void Face::Update() {
  if (_eyes) _eyes->update();
}

// Moods
void Face::Happy()   { if (_eyes) _eyes->setMood(HAPPY); }
void Face::Angry()   { if (_eyes) _eyes->setMood(ANGRY); }
void Face::Tired()   { if (_eyes) _eyes->setMood(TIRED); }
void Face::Neutral() { if (_eyes) _eyes->setMood(DEFAULT); }

// Looking
void Face::LookCenter() { if (_eyes) _eyes->setPosition(DEFAULT); }
void Face::LookUp()     { if (_eyes) _eyes->setPosition(N); }
void Face::LookDown()   { if (_eyes) _eyes->setPosition(S); }
void Face::LookLeft()   { if (_eyes) _eyes->setPosition(W); }
void Face::LookRight()  { if (_eyes) _eyes->setPosition(E); }

// Animations
void Face::Blink()     { if (_eyes) _eyes->blink(); }
void Face::WinkLeft()  { if (_eyes) _eyes->blink(1, 0); }
void Face::WinkRight() { if (_eyes) _eyes->blink(0, 1); }
void Face::Laugh()     { if (_eyes) _eyes->anim_laugh(); }
void Face::Confused()  { if (_eyes) _eyes->anim_confused(); }

// Modes
void Face::SetCyclops(bool on) { if (_eyes) _eyes->setCyclops(on ? ON : OFF); }
void Face::SetSweat(bool on)   { if (_eyes) _eyes->setSweat(on ? ON : OFF); }
void Face::SetCurious(bool on) { if (_eyes) _eyes->setCuriosity(on ? ON : OFF); }
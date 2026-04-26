#include "Face.h"
#include <Wire.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define FRAMERATE 50
#define BUZZER_PIN 5

Face::Face() 
  : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1),
    _eyes(nullptr),
    _autonomous(false),
    _moodChangeTimer(0),
    _reactionTimer(0),
    _currentMoodPhase(0),
    _winkScheduledAt(0),
    _winkPending(false),
    _sound(BUZZER_PIN),
    _soundEnabled(true)
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

  _sound.Init();
  return true;
}

void Face::Update() {
  if (_autonomous) _autonomousTick();

  if (_winkPending && millis() >= _winkScheduledAt) {
    if (_eyes) _eyes->blink(0, 1);
    _winkPending = false;
  }

  _sound.Update();
  if (_eyes) _eyes->update();
}

void Face::EnableSound(bool on) {
  _soundEnabled = on;
  if (!on) _sound.Stop();
}

void Face::TriggerRandomEmotion() {
  // Temporarily enable sound just for this one emotion
  bool wasEnabled = _soundEnabled;
  _soundEnabled = true;

  int newPhase = random(17);
  while (newPhase == _currentMoodPhase) {
    newPhase = random(17);
  }
  _currentMoodPhase = newPhase;

  switch (newPhase) {
    case 0:  Happy();       break;
    case 1:  Neutral();     break;
    case 2:  Tired();       break;
    case 3:  Suspicious();  break;
    case 4:  Sleepy();      break;
    case 5:  Surprised();   break;
    case 6:  Excited();     break;
    case 7:  Sad();         break;
    case 8:  Bored();       break;
    case 9:  Anxious();     break;
    case 10: InLove();      break;
    case 11: Scared();      break;
    case 12: Skeptical();   break;
    case 13: Dazed();       break;
    case 14: Mischievous(); break;
    case 15: Cute();        break;
    case 16: Mean();        break;
  }

  _soundEnabled = wasEnabled;   // restore previous setting (silent again)

  // Reset the autonomous mood-change timer so it doesn't immediately override
  if (_autonomous) {
    _moodChangeTimer = millis() + 12000 + random(18000);
  }
}

void Face::SetEyeSize(int width, int height) {
  if (_eyes) {
    _eyes->setWidth(width, width);
    _eyes->setHeight(height, height);
  }
}

void Face::SetEyeRoundness(int radius) {
  if (_eyes) _eyes->setBorderradius(radius, radius);
}

void Face::ResetEyeShape() {
  if (!_eyes) return;
  _resetEffects();
  _eyes->setWidth(36, 36);
  _eyes->setHeight(36, 36);
  _eyes->setBorderradius(6, 6);
  _eyes->setSpacebetween(10);
}

void Face::AutonomousMode(bool on) {
  _autonomous = on;
  if (on) {
    _moodChangeTimer = millis() + 5000;
    _reactionTimer = millis() + 4000;

    if (_eyes) {
      _eyes->setAutoblinker(ON, 2, 4);
      _eyes->setIdleMode(ON, 1, 3);
    }
  }
}

void Face::_autonomousTick() {
  unsigned long now = millis();

  // Mood phase changes (every 12-30 seconds)
  if (now >= _moodChangeTimer) {
    int newPhase = random(17);

    while (newPhase == _currentMoodPhase) {
      newPhase = random(17);
    }
    _currentMoodPhase = newPhase;
    unsigned long duration = 12000 + random(18000);   // default for everyone

    switch (newPhase)
    {
    case 0:
        Happy();
        duration = 60000 + random(60000);
        break;
    case 1:
        Neutral();
        duration = 60000 + random(60000);
        break;
    case 2:
        Tired();
        break;
    case 3:
        Suspicious();
        break;
    case 4:
        Sleepy();
        break;
    case 5:
        Surprised();
        break;
    case 6:
        Excited();
        break;
    case 7:
        Sad();
        break;
    case 8:
        Bored();
        break;
    case 9:
        Anxious();
        break;
    case 10:
        InLove();
        break;
    case 11:
        Scared();
        break;
    case 12:
        Skeptical();
        break;
    case 13:
        Dazed();
        break;
    case 14:
        Mischievous();
        break;
    case 15:
        Cute();
        break;
    case 16:
        Mean();
        break;
    }

    _moodChangeTimer = now + duration;
  }

  // Random small reactions
  if (now >= _reactionTimer) {
    int reaction = random(8);

    switch (reaction) {
      case 0: Laugh();    break;
      case 1: Confused(); break;
      case 2: WinkLeft(); break;
      case 3: WinkRight();break;
      case 4: Blink();    break;
      case 5: LookUp();   break;
      case 6: LookDown(); break;
    }

    _reactionTimer = now + 6000 + random(9000);
  }
}

void Face::_resetEffects() {
  if (!_eyes) return;
  _eyes->setHFlicker(OFF, 0);
  _eyes->setVFlicker(OFF, 0);
  _eyes->setSweat(OFF);
  _eyes->setCuriosity(OFF);
  _winkPending = false;       // cancel any pending wink from a previous emotion
}

// ============== BASE METHODS (with sound) ==============

void Face::Happy() {
    _resetEffects();
  if (_eyes) _eyes->setMood(HAPPY);
  if (_soundEnabled) _sound.Happy();
}

void Face::Angry() {
    _resetEffects();
  if (_eyes) _eyes->setMood(ANGRY);
  if (_soundEnabled) _sound.Angry();
}

void Face::Tired() {
    _resetEffects();
  if (_eyes) _eyes->setMood(TIRED);
  if (_soundEnabled) _sound.Tired();
}

void Face::Neutral() {
    _resetEffects();
  if (_eyes) _eyes->setMood(DEFAULT);
  // no sound for neutral — quiet idle
}

void Face::LookCenter() { if (_eyes) _eyes->setPosition(DEFAULT); }
void Face::LookUp()     { if (_eyes) _eyes->setPosition(N); }
void Face::LookDown()   { if (_eyes) _eyes->setPosition(S); }
void Face::LookLeft()   { if (_eyes) _eyes->setPosition(W); }
void Face::LookRight()  { if (_eyes) _eyes->setPosition(E); }

void Face::Blink() {
  if (_eyes) _eyes->blink();
}

void Face::WinkLeft() {
  if (_eyes) _eyes->blink(1, 0);
  if (_soundEnabled) _sound.Beep();
}

void Face::WinkRight() {
  if (_eyes) _eyes->blink(0, 1);
  if (_soundEnabled) _sound.Beep();
}

void Face::Laugh() {
  if (_eyes) _eyes->anim_laugh();
  if (_soundEnabled) _sound.Laugh();
}

void Face::Confused() {
  if (_eyes) _eyes->anim_confused();
  if (_soundEnabled) _sound.Confused();
}

void Face::SetCyclops(bool on) { if (_eyes) _eyes->setCyclops(on ? ON : OFF); }
void Face::SetSweat(bool on)   { if (_eyes) _eyes->setSweat(on ? ON : OFF); }
void Face::SetCurious(bool on) { if (_eyes) _eyes->setCuriosity(on ? ON : OFF); }


// ============== COMPOUND EMOTIONS (each with its own sound) ==============

// SUSPICIOUS — angry + side glance + smaller eyes
void Face::Suspicious() {
  ResetEyeShape();
  Angry();                                // sets angry mood + plays angry sound
  SetEyeSize(32, 28);
  SetEyeRoundness(4);
  LookRight();
  SetSweat(false);
  SetCurious(true);
  if (_soundEnabled) _sound.Mischievous();  // override with suspicious-ish sound
}

// SLEEPY — tired + drooping + smaller eyes
void Face::Sleepy() {
  ResetEyeShape();
  Tired();
  SetEyeSize(36, 22);
  SetEyeRoundness(8);
  LookDown();
  SetSweat(false);
  if (_soundEnabled) _sound.Sleepy();
}

// SURPRISED — taller and wider, robot-style
void Face::Surprised() {
  ResetEyeShape();
  Neutral();
  SetEyeSize(42, 44);
  SetEyeRoundness(6);
  LookUp();
  SetCurious(false);
  SetSweat(false);
  if (_soundEnabled) _sound.Surprised();
}

// EXCITED — happy + laugh + slightly bigger
void Face::Excited() {
  ResetEyeShape();
  Happy();
  SetEyeSize(38, 38);
  SetEyeRoundness(8);
  LookCenter();
  Laugh();
  SetCurious(true);
  if (_soundEnabled) _sound.Excited();
}

// SAD — tired + look down + smaller eyes
void Face::Sad() {
  ResetEyeShape();
  Tired();
  SetEyeSize(30, 26);
  SetEyeRoundness(6);
  LookDown();
  SetSweat(false);
  if (_soundEnabled) _sound.Sad();
}

// BORED — half-lidded + look sideways
void Face::Bored() {
  ResetEyeShape();
  Tired();
  SetEyeSize(36, 20);
  SetEyeRoundness(6);
  LookLeft();
  SetCurious(false);
  if (_soundEnabled) _sound.Tired();   // bored shares the tired sigh
}

// ANXIOUS — angry + sweat + small eyes
void Face::Anxious() {
  ResetEyeShape();
  Angry();
  SetEyeSize(30, 30);
  SetEyeRoundness(6);
  SetSweat(true);
  LookCenter();
  if (_soundEnabled) _sound.Scared();   // nervous stutter
}

// IN LOVE — happy + sweat (floating bubbles)
void Face::InLove() {
  ResetEyeShape();
  Happy();
  SetEyeSize(36, 36);
  SetEyeRoundness(8);
  SetSweat(true);
  LookCenter();
  SetCurious(true);
  if (_soundEnabled) _sound.InLove();
}

// SCARED — wider eyes + nervous shake
void Face::Scared() {
  ResetEyeShape();
  Neutral();
  SetEyeSize(40, 40);
  SetEyeRoundness(6);
  SetSweat(true);
  LookUp();
  if (_eyes) _eyes->setHFlicker(ON, 2);
  if (_soundEnabled) _sound.Scared();
}

// SKEPTICAL — angry + raised brow effect (asymmetric eyes)
void Face::Skeptical() {
  ResetEyeShape();
  Angry();
  if (_eyes) {
    _eyes->setHeight(28, 36);
    _eyes->setWidth(34, 36);
    _eyes->setBorderradius(4, 4);
  }
  LookLeft();
  SetSweat(false);
  SetCurious(false);
  if (_soundEnabled) _sound.Curious();   // questioning chirp
}

// DAZED — small + wobble
void Face::Dazed() {
  ResetEyeShape();
  Neutral();
  SetEyeSize(28, 28);
  SetEyeRoundness(4);
  if (_eyes) _eyes->setVFlicker(ON, 3);
  SetSweat(false);
  if (_soundEnabled) _sound.Confused();   // wavering warble
}

// MISCHIEVOUS — happy + side glance + delayed wink
void Face::Mischievous() {
  ResetEyeShape();
  Happy();
  SetEyeSize(36, 32);
  SetEyeRoundness(6);
  LookRight();
  SetCurious(true);
  _winkScheduledAt = millis() + 800;
  _winkPending = true;
  if (_soundEnabled) _sound.Mischievous();
}

// CUTE — slightly bigger, soft but rectangular
void Face::Cute() {
  ResetEyeShape();
  Happy();
  SetEyeSize(40, 36);
  SetEyeRoundness(10);
  LookCenter();
  SetCurious(false);
  SetSweat(false);
  if (_soundEnabled) _sound.Cute();
}

// MEAN — small angry eyes + side look + sharp
void Face::Mean() {
  ResetEyeShape();
  Angry();
  SetEyeSize(26, 22);
  SetEyeRoundness(2);
  LookRight();
  SetSweat(false);
  if (_soundEnabled) _sound.Mean();
}

// CRYING — tired + tears + look down
void Face::Crying() {
  ResetEyeShape();
  Tired();
  SetEyeSize(34, 24);
  SetEyeRoundness(6);
  SetSweat(true);
  LookDown();
  if (_soundEnabled) _sound.Sad();
}

// SNEAKY — squinty + side eye + wink
void Face::Sneaky() {
  ResetEyeShape();
  Happy();
  SetEyeSize(32, 28);
  SetEyeRoundness(4);
  LookLeft();
  SetCurious(true);
  _winkScheduledAt = millis() + 600;
  _winkPending = true;
  if (_soundEnabled) _sound.Mischievous();
}
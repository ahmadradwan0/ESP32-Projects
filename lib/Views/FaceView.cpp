#include "FaceView.h"
#include <FluxGarage_RoboEyes.h>

// These match the big OLED that ScreenManager owns. Kept as named constants
// so the RoboEyes coordinate space is explicit at the call site.
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define FRAMERATE     50

// ============================================================
//  Construction / binding / lifecycle
// ============================================================

FaceView::FaceView(ISound& sound)
  : _sound(sound),
    _soundEnabled(true),
    _eyes(nullptr),
    _winkScheduledAt(0),
    _winkPending(false),
    _autonomous(false),
    _moodChangeTimer(0),
    _reactionTimer(0),
    _currentMoodPhase(0)
{
}

FaceView::~FaceView() {
  delete _eyes;
}

void FaceView::Bind(Adafruit_SSD1306& display) {
  if (_eyes != nullptr) return;   // bind-once: ignore subsequent calls

  _eyes = new RoboEyes<Adafruit_SSD1306>(display);
  _eyes->begin(SCREEN_WIDTH, SCREEN_HEIGHT, FRAMERATE);
  _eyes->setAutoblinker(ON, 3, 2);
  _eyes->setIdleMode(ON, 2, 2);
}

void FaceView::Draw(Adafruit_SSD1306& display) {
  (void)display;   // _eyes already references the bound display
  if (!_eyes) return;

  if (_autonomous) _autonomousTick();

  if (_winkPending && millis() >= _winkScheduledAt) {
    _eyes->blink(0, 1);
    _winkPending = false;
  }

  _eyes->update();
}

// ============================================================
//  Sound gating
// ============================================================

void FaceView::EnableSound(bool on) {
  _soundEnabled = on;
  if (!on) _sound.Stop();
}

// ============================================================
//  Random emotion (button-driven)
// ============================================================

void FaceView::TriggerRandomEmotion() {
  // Always sound for explicit user-triggered emotion, regardless of the
  // ambient mute state.
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

  _soundEnabled = wasEnabled;

  // Don't let autonomous tick stomp on the user's pick immediately
  if (_autonomous) {
    _moodChangeTimer = millis() + 12000 + random(18000);
  }
}

// ============================================================
//  Eye shape helpers
// ============================================================

void FaceView::SetEyeSize(int width, int height) {
  if (_eyes) {
    _eyes->setWidth(width, width);
    _eyes->setHeight(height, height);
  }
}

void FaceView::SetEyeRoundness(int radius) {
  if (_eyes) _eyes->setBorderradius(radius, radius);
}

void FaceView::ResetEyeShape() {
  if (!_eyes) return;
  _resetEffects();
  _eyes->setWidth(36, 36);
  _eyes->setHeight(36, 36);
  _eyes->setBorderradius(6, 6);
  _eyes->setSpacebetween(10);
}

void FaceView::_resetEffects() {
  if (!_eyes) return;
  _eyes->setHFlicker(OFF, 0);
  _eyes->setVFlicker(OFF, 0);
  _eyes->setSweat(OFF);
  _eyes->setCuriosity(OFF);
  _winkPending = false;   // cancel any pending wink from a prior emotion
}

// ============================================================
//  Autonomous mode
// ============================================================

void FaceView::AutonomousMode(bool on) {
  _autonomous = on;
  if (on) {
    _moodChangeTimer = millis() + 5000;
    _reactionTimer   = millis() + 4000;

    if (_eyes) {
      _eyes->setAutoblinker(ON, 2, 4);
      _eyes->setIdleMode(ON, 1, 3);
    }
  }
}

void FaceView::_autonomousTick() {
  unsigned long now = millis();

  // Mood phase changes (every ~12-30 seconds, longer for happy/neutral)
  if (now >= _moodChangeTimer) {
    int newPhase = random(17);
    while (newPhase == _currentMoodPhase) {
      newPhase = random(17);
    }
    _currentMoodPhase = newPhase;

    unsigned long duration = 12000 + random(18000);

    switch (newPhase) {
      case 0:  Happy();       duration = 60000 + random(60000); break;
      case 1:  Neutral();     duration = 60000 + random(60000); break;
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

    _moodChangeTimer = now + duration;
  }

  // Small reactions sprinkled in between mood changes
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

// ============================================================
//  Base moods
// ============================================================

void FaceView::Happy() {
  _resetEffects();
  if (_eyes) _eyes->setMood(HAPPY);
  if (_soundEnabled) _sound.Happy();
}

void FaceView::Angry() {
  _resetEffects();
  if (_eyes) _eyes->setMood(ANGRY);
  if (_soundEnabled) _sound.Angry();
}

void FaceView::Tired() {
  _resetEffects();
  if (_eyes) _eyes->setMood(TIRED);
  if (_soundEnabled) _sound.Tired();
}

void FaceView::Neutral() {
  _resetEffects();
  if (_eyes) _eyes->setMood(DEFAULT);
  // intentionally silent — quiet idle
}

// ============================================================
//  Looking
// ============================================================

void FaceView::LookCenter() { if (_eyes) _eyes->setPosition(DEFAULT); }
void FaceView::LookUp()     { if (_eyes) _eyes->setPosition(N); }
void FaceView::LookDown()   { if (_eyes) _eyes->setPosition(S); }
void FaceView::LookLeft()   { if (_eyes) _eyes->setPosition(W); }
void FaceView::LookRight()  { if (_eyes) _eyes->setPosition(E); }

// ============================================================
//  One-shot animations
// ============================================================

void FaceView::Blink() {
  if (_eyes) _eyes->blink();
}

void FaceView::WinkLeft() {
  if (_eyes) _eyes->blink(1, 0);
  if (_soundEnabled) _sound.Beep();
}

void FaceView::WinkRight() {
  if (_eyes) _eyes->blink(0, 1);
  if (_soundEnabled) _sound.Beep();
}

void FaceView::Laugh() {
  if (_eyes) _eyes->anim_laugh();
  if (_soundEnabled) _sound.Laugh();
}

void FaceView::Confused() {
  if (_eyes) _eyes->anim_confused();
  if (_soundEnabled) _sound.Confused();
}

// ============================================================
//  Effect toggles
// ============================================================

void FaceView::SetCyclops(bool on) { if (_eyes) _eyes->setCyclops(on ? ON : OFF); }
void FaceView::SetSweat(bool on)   { if (_eyes) _eyes->setSweat(on ? ON : OFF); }
void FaceView::SetCurious(bool on) { if (_eyes) _eyes->setCuriosity(on ? ON : OFF); }

// ============================================================
//  Compound emotions — each composed from the primitives above
// ============================================================

// SUSPICIOUS — angry + side glance + smaller eyes
void FaceView::Suspicious() {
  ResetEyeShape();
  Angry();
  SetEyeSize(32, 28);
  SetEyeRoundness(4);
  LookRight();
  SetSweat(false);
  SetCurious(true);
  if (_soundEnabled) _sound.Mischievous();
}

// SLEEPY — tired + drooping + smaller eyes
void FaceView::Sleepy() {
  ResetEyeShape();
  Tired();
  SetEyeSize(36, 22);
  SetEyeRoundness(8);
  LookDown();
  SetSweat(false);
  if (_soundEnabled) _sound.Sleepy();
}

// SURPRISED — taller and wider, robot-style
void FaceView::Surprised() {
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
void FaceView::Excited() {
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
void FaceView::Sad() {
  ResetEyeShape();
  Tired();
  SetEyeSize(30, 26);
  SetEyeRoundness(6);
  LookDown();
  SetSweat(false);
  if (_soundEnabled) _sound.Sad();
}

// BORED — half-lidded + look sideways
void FaceView::Bored() {
  ResetEyeShape();
  Tired();
  SetEyeSize(36, 20);
  SetEyeRoundness(6);
  LookLeft();
  SetCurious(false);
  if (_soundEnabled) _sound.Tired();   // bored shares the tired sigh
}

// ANXIOUS — angry + sweat + small eyes
void FaceView::Anxious() {
  ResetEyeShape();
  Angry();
  SetEyeSize(30, 30);
  SetEyeRoundness(6);
  SetSweat(true);
  LookCenter();
  if (_soundEnabled) _sound.Scared();   // nervous stutter
}

// IN LOVE — happy + sweat + curious bubbles
void FaceView::InLove() {
  ResetEyeShape();
  Happy();
  SetEyeSize(36, 36);
  SetEyeRoundness(8);
  SetSweat(true);
  LookCenter();
  SetCurious(true);
  if (_soundEnabled) _sound.InLove();
}

// SCARED — wide eyes + nervous shake
void FaceView::Scared() {
  ResetEyeShape();
  Neutral();
  SetEyeSize(40, 40);
  SetEyeRoundness(6);
  SetSweat(true);
  LookUp();
  if (_eyes) _eyes->setHFlicker(ON, 2);
  if (_soundEnabled) _sound.Scared();
}

// SKEPTICAL — angry + asymmetric eyes (raised brow effect)
void FaceView::Skeptical() {
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

// DAZED — small + vertical wobble
void FaceView::Dazed() {
  ResetEyeShape();
  Neutral();
  SetEyeSize(28, 28);
  SetEyeRoundness(4);
  if (_eyes) _eyes->setVFlicker(ON, 3);
  SetSweat(false);
  if (_soundEnabled) _sound.Confused();   // wavering warble
}

// MISCHIEVOUS — happy + side glance + delayed wink
void FaceView::Mischievous() {
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

// CUTE — slightly bigger, soft but still rectangular
void FaceView::Cute() {
  ResetEyeShape();
  Happy();
  SetEyeSize(40, 36);
  SetEyeRoundness(10);
  LookCenter();
  SetCurious(false);
  SetSweat(false);
  if (_soundEnabled) _sound.Cute();
}

// MEAN — small angry eyes + side look + sharp corners
void FaceView::Mean() {
  ResetEyeShape();
  Angry();
  SetEyeSize(26, 22);
  SetEyeRoundness(2);
  LookRight();
  SetSweat(false);
  if (_soundEnabled) _sound.Mean();
}

// CRYING — tired + tears + look down
void FaceView::Crying() {
  ResetEyeShape();
  Tired();
  SetEyeSize(34, 24);
  SetEyeRoundness(6);
  SetSweat(true);
  LookDown();
  if (_soundEnabled) _sound.Sad();
}

// SNEAKY — squinty + side eye + delayed wink
void FaceView::Sneaky() {
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
#include "Sound.h"

Sound::Sound(int pin) : _pin(pin), _queueLen(0), _queueIdx(0), _stepStartMs(0) {}

void Sound::Init() {
  noTone(_pin);
}

void Sound::Stop() {
  noTone(_pin);
  _queueLen = 0;
  _queueIdx = 0;
}

void Sound::_clear() {
  noTone(_pin);
  _queueLen = 0;
  _queueIdx = 0;
  _stepStartMs = 0;
}

void Sound::_add(int startFreq, int endFreq, int durationMs) {
  if (_queueLen < SOUND_QUEUE_SIZE) {
    _queue[_queueLen].startFreq = startFreq;
    _queue[_queueLen].endFreq = endFreq;
    _queue[_queueLen].durationMs = durationMs;
    _queueLen++;
  }
}

void Sound::_addNote(int freq, int durationMs) {
  _add(freq, freq, durationMs);
}

void Sound::_addSweep(int startFreq, int endFreq, int durationMs) {
  _add(startFreq, endFreq, durationMs);
}

void Sound::_addPause(int durationMs) {
  _add(0, 0, durationMs);
}

// Multiple short chirps with random-ish frequencies near baseFreq
// This is the "R2-D2 chatter" pattern
void Sound::_addChirpBurst(int baseFreq, int variation, int chirps, int eachMs) {
  for (int i = 0; i < chirps; i++) {
    int start = baseFreq + random(-variation, variation);
    int end = start + random(-variation/2, variation);
    _addSweep(start, end, eachMs);
    if (i < chirps - 1) _addPause(eachMs / 4);
  }
}

void Sound::Update() {
  unsigned long now = millis();

  if (_queueIdx >= _queueLen) {
    if (_queueLen > 0) {
      noTone(_pin);
      _queueLen = 0;
      _queueIdx = 0;
    }
    return;
  }

  Step& s = _queue[_queueIdx];

  if (_stepStartMs == 0) {
    // Starting this step
    if (s.startFreq == 0) {
      noTone(_pin);
    } else {
      tone(_pin, s.startFreq);
    }
    _stepStartMs = now;
    return;
  }

  unsigned long elapsed = now - _stepStartMs;

  // If it's a sweep (different start/end freq), update tone continuously
  if (s.startFreq != s.endFreq && s.startFreq != 0 && elapsed < (unsigned long)s.durationMs) {
    int freq = s.startFreq + ((s.endFreq - s.startFreq) * (int)elapsed) / s.durationMs;
    tone(_pin, freq);
  }

  // Step finished?
  if (elapsed >= (unsigned long)s.durationMs) {
    _queueIdx++;
    _stepStartMs = 0;
  }
}

// =========================================================
//  EMOTION SOUNDS — R2-D2 / robotic style
// =========================================================

// HAPPY — 4 ascending chirpy bursts
void Sound::Happy() {
  _clear();
  _addSweep(500, 700, 60);
  _addPause(20);
  _addSweep(700, 900, 60);
  _addPause(20);
  _addSweep(900, 1100, 60);
  _addPause(20);
  _addSweep(1100, 1400, 100);
}

// SAD — slow descending sweep with wobble
void Sound::Sad() {
  _clear();
  _addSweep(600, 400, 250);
  _addSweep(400, 350, 100);
  _addSweep(350, 200, 400);
}

// ANGRY — harsh low stutter with descending growl
void Sound::Angry() {
  _clear();
  _addNote(150, 80);
  _addPause(30);
  _addNote(120, 80);
  _addPause(30);
  _addSweep(180, 80, 200);
}

// TIRED — long sigh slide
void Sound::Tired() {
  _clear();
  _addSweep(500, 200, 700);
}

// SURPRISED — sudden steep upward chirp + question chirp
void Sound::Surprised() {
  _clear();
  _addSweep(300, 1500, 120);
  _addPause(40);
  _addSweep(1200, 1500, 80);
}

// EXCITED — fast random chirp burst (true R2-D2 chatter)
void Sound::Excited() {
  _clear();
  _addChirpBurst(1000, 300, 6, 50);
}

// CONFUSED — wavering up-down warble
void Sound::Confused() {
  _clear();
  _addSweep(700, 400, 80);
  _addSweep(400, 800, 80);
  _addSweep(800, 500, 80);
  _addSweep(500, 650, 100);
}

// LAUGH — bouncy "hee hee hee" with sweeps
void Sound::Laugh() {
  _clear();
  for (int i = 0; i < 4; i++) {
    _addSweep(800, 1000, 50);
    _addPause(40);
  }
}

// CURIOUS — questioning rising chirp
void Sound::Curious() {
  _clear();
  _addSweep(500, 700, 80);
  _addSweep(700, 1000, 150);
}

// SLEEPY — soft slow descending breath
void Sound::Sleepy() {
  _clear();
  _addSweep(450, 300, 400);
  _addPause(80);
  _addSweep(350, 200, 400);
}

// SCARED — fast nervous high stutter
void Sound::Scared() {
  _clear();
  for (int i = 0; i < 5; i++) {
    _addNote(900 + (i * 100), 30);
    _addPause(20);
  }
  _addSweep(1300, 1500, 120);
}

// IN LOVE — sweet rising chirps with sigh
void Sound::InLove() {
  _clear();
  _addSweep(600, 800, 100);
  _addPause(30);
  _addSweep(800, 1000, 100);
  _addPause(50);
  _addSweep(1000, 700, 200);
}

// MISCHIEVOUS — sneaky descending warble
void Sound::Mischievous() {
  _clear();
  _addSweep(700, 500, 80);
  _addSweep(500, 700, 60);
  _addSweep(700, 400, 200);
}

// CUTE — soft 2-step rising chirp
void Sound::Cute() {
  _clear();
  _addSweep(700, 900, 80);
  _addPause(40);
  _addSweep(900, 1100, 120);
}

// MEAN — low descending growl
void Sound::Mean() {
  _clear();
  _addSweep(250, 100, 350);
  _addNote(80, 200);
}

// =========================================================
//  GENERIC ACTIONS
// =========================================================

void Sound::Beep() {
  _clear();
  _addSweep(800, 1000, 60);
}

void Sound::DoubleBeep() {
  _clear();
  _addNote(900, 50);
  _addPause(40);
  _addNote(900, 50);
}

void Sound::Chirp() {
  _clear();
  _addSweep(600, 1300, 120);
}

void Sound::Trill() {
  _clear();
  for (int i = 0; i < 4; i++) {
    _addNote(800, 30);
    _addNote(1000, 30);
  }
}
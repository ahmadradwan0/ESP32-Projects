#include "BuzzerSound.h"

BuzzerSound::BuzzerSound(int pin) : _pin(pin), _queueLen(0), _queueIdx(0), _stepStartMs(0) {}

void BuzzerSound::Init() {
  noTone(_pin);
}

void BuzzerSound::Stop() {
  noTone(_pin);
  _queueLen = 0;
  _queueIdx = 0;
}

void BuzzerSound::_clear() {
  noTone(_pin);
  _queueLen = 0;
  _queueIdx = 0;
  _stepStartMs = 0;
}

void BuzzerSound::_add(int startFreq, int endFreq, int durationMs) {
  if (_queueLen < BUZZER_SOUND_QUEUE_SIZE) {
    _queue[_queueLen].startFreq = startFreq;
    _queue[_queueLen].endFreq = endFreq;
    _queue[_queueLen].durationMs = durationMs;
    _queueLen++;
  }
}

void BuzzerSound::_addNote(int freq, int durationMs) {
  _add(freq, freq, durationMs);
}

void BuzzerSound::_addSweep(int startFreq, int endFreq, int durationMs) {
  _add(startFreq, endFreq, durationMs);
}

void BuzzerSound::_addPause(int durationMs) {
  _add(0, 0, durationMs);
}

// Multiple short chirps with random-ish frequencies near baseFreq
// This is the "R2-D2 chatter" pattern
void BuzzerSound::_addChirpBurst(int baseFreq, int variation, int chirps, int eachMs) {
  for (int i = 0; i < chirps; i++) {
    int start = baseFreq + random(-variation, variation);
    int end = start + random(-variation/2, variation);
    _addSweep(start, end, eachMs);
    if (i < chirps - 1) _addPause(eachMs / 4);
  }
}

void BuzzerSound::Update() {
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
    if (s.startFreq == 0) {
      noTone(_pin);
    } else {
      tone(_pin, s.startFreq);
    }
    _stepStartMs = now;
    return;
  }

  unsigned long elapsed = now - _stepStartMs;

  if (s.startFreq != s.endFreq && s.startFreq != 0 && elapsed < (unsigned long)s.durationMs) {
    int freq = s.startFreq + ((s.endFreq - s.startFreq) * (int)elapsed) / s.durationMs;
    tone(_pin, freq);
  }

  if (elapsed >= (unsigned long)s.durationMs) {
    _queueIdx++;
    _stepStartMs = 0;
  }
}

// =========================================================
//  EMOTION SOUNDS — R2-D2 / robotic style
// =========================================================

void BuzzerSound::Happy() {
  _clear();
  _addSweep(500, 700, 60);
  _addPause(20);
  _addSweep(700, 900, 60);
  _addPause(20);
  _addSweep(900, 1100, 60);
  _addPause(20);
  _addSweep(1100, 1400, 100);
}

void BuzzerSound::Sad() {
  _clear();
  _addSweep(600, 400, 250);
  _addSweep(400, 350, 100);
  _addSweep(350, 200, 400);
}

void BuzzerSound::Angry() {
  _clear();
  _addNote(150, 80);
  _addPause(30);
  _addNote(120, 80);
  _addPause(30);
  _addSweep(180, 80, 200);
}

void BuzzerSound::Tired() {
  _clear();
  _addSweep(500, 200, 700);
}

void BuzzerSound::Surprised() {
  _clear();
  _addSweep(300, 1500, 120);
  _addPause(40);
  _addSweep(1200, 1500, 80);
}

void BuzzerSound::Excited() {
  _clear();
  _addChirpBurst(1000, 300, 6, 50);
}

void BuzzerSound::Confused() {
  _clear();
  _addSweep(700, 400, 80);
  _addSweep(400, 800, 80);
  _addSweep(800, 500, 80);
  _addSweep(500, 650, 100);
}

void BuzzerSound::Laugh() {
  _clear();
  for (int i = 0; i < 4; i++) {
    _addSweep(800, 1000, 50);
    _addPause(40);
  }
}

void BuzzerSound::Curious() {
  _clear();
  _addSweep(500, 700, 80);
  _addSweep(700, 1000, 150);
}

void BuzzerSound::Sleepy() {
  _clear();
  _addSweep(450, 300, 400);
  _addPause(80);
  _addSweep(350, 200, 400);
}

void BuzzerSound::Scared() {
  _clear();
  for (int i = 0; i < 5; i++) {
    _addNote(900 + (i * 100), 30);
    _addPause(20);
  }
  _addSweep(1300, 1500, 120);
}

void BuzzerSound::InLove() {
  _clear();
  _addSweep(600, 800, 100);
  _addPause(30);
  _addSweep(800, 1000, 100);
  _addPause(50);
  _addSweep(1000, 700, 200);
}

void BuzzerSound::Mischievous() {
  _clear();
  _addSweep(700, 500, 80);
  _addSweep(500, 700, 60);
  _addSweep(700, 400, 200);
}

void BuzzerSound::Cute() {
  _clear();
  _addSweep(700, 900, 80);
  _addPause(40);
  _addSweep(900, 1100, 120);
}

void BuzzerSound::Mean() {
  _clear();
  _addSweep(250, 100, 350);
  _addNote(80, 200);
}

// =========================================================
//  GENERIC ACTIONS
// =========================================================

void BuzzerSound::Beep() {
  _clear();
  _addSweep(800, 1000, 60);
}

void BuzzerSound::DoubleBeep() {
  _clear();
  _addNote(900, 50);
  _addPause(40);
  _addNote(900, 50);
}

void BuzzerSound::Chirp() {
  _clear();
  _addSweep(600, 1300, 120);
}

void BuzzerSound::Trill() {
  _clear();
  for (int i = 0; i < 4; i++) {
    _addNote(800, 30);
    _addNote(1000, 30);
  }
}
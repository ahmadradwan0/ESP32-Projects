#pragma once
#include <Arduino.h>

#define SOUND_QUEUE_SIZE 32

class Sound {
  public:
    Sound(int pin);
    void Init();
    void Update();
    void Stop();

    // Emotion sounds (now more robotic)
    void Happy();
    void Sad();
    void Angry();
    void Tired();
    void Surprised();
    void Excited();
    void Confused();
    void Laugh();
    void Curious();
    void Sleepy();
    void Scared();
    void InLove();
    void Mischievous();
    void Cute();
    void Mean();

    void Beep();
    void DoubleBeep();
    void Chirp();
    void Trill();

  private:
    int _pin;

    // Each step is either a flat tone OR a sweep
    struct Step {
      int startFreq;     // 0 = silence
      int endFreq;       // if same as start, it's a flat tone; otherwise sweep
      int durationMs;
    };
    Step _queue[SOUND_QUEUE_SIZE];
    int _queueLen;
    int _queueIdx;
    unsigned long _stepStartMs;

    // Helpers
    void _clear();
    void _add(int startFreq, int endFreq, int durationMs);
    void _addNote(int freq, int durationMs);
    void _addSweep(int startFreq, int endFreq, int durationMs);
    void _addPause(int durationMs);
    void _addChirpBurst(int baseFreq, int variation, int chirps, int eachMs);
};
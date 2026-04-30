#pragma once
#include "ISound.h"

#define BUZZER_SOUND_QUEUE_SIZE 32

class BuzzerSound : public ISound {
  public:
    BuzzerSound(int pin);

    // ISound contract
    void Init() override;
    void Update() override;
    void Stop() override;

    // Emotion sounds
    void Happy() override;
    void Sad() override;
    void Angry() override;
    void Tired() override;
    void Surprised() override;
    void Excited() override;
    void Confused() override;
    void Laugh() override;
    void Curious() override;
    void Sleepy() override;
    void Scared() override;
    void InLove() override;
    void Mischievous() override;
    void Cute() override;
    void Mean() override;

    void Beep() override;
    void DoubleBeep() override;
    void Chirp() override;
    void Trill() override;

  private:
    int _pin;

    struct Step {
      int startFreq;
      int endFreq;
      int durationMs;
    };
    Step _queue[BUZZER_SOUND_QUEUE_SIZE];
    int _queueLen;
    int _queueIdx;
    unsigned long _stepStartMs;

    void _clear();
    void _add(int startFreq, int endFreq, int durationMs);
    void _addNote(int freq, int durationMs);
    void _addSweep(int startFreq, int endFreq, int durationMs);
    void _addPause(int durationMs);
    void _addChirpBurst(int baseFreq, int variation, int chirps, int eachMs);
};
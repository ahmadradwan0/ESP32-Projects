#pragma once
#include <Arduino.h>

// ISound — the contract every sound system implements.
//
// Used by views (Face, Weather, etc.) to play sounds without knowing
// which physical sound system is underneath. Could be a buzzer, an I2S
// speaker, or anything else that can make noise.
//
// Views receive ISound& by reference (dependency injection).
class ISound {
  public:
    virtual ~ISound() {}

    // Lifecycle
    virtual void Init() = 0;
    virtual void Update() = 0;     // call every loop
    virtual void Stop() = 0;

    // Emotion sounds
    virtual void Happy() = 0;
    virtual void Sad() = 0;
    virtual void Angry() = 0;
    virtual void Tired() = 0;
    virtual void Surprised() = 0;
    virtual void Excited() = 0;
    virtual void Confused() = 0;
    virtual void Laugh() = 0;
    virtual void Curious() = 0;
    virtual void Sleepy() = 0;
    virtual void Scared() = 0;
    virtual void InLove() = 0;
    virtual void Mischievous() = 0;
    virtual void Cute() = 0;
    virtual void Mean() = 0;

    // Generic actions
    virtual void Beep() = 0;
    virtual void DoubleBeep() = 0;
    virtual void Chirp() = 0;
    virtual void Trill() = 0;
    virtual void ClockChimes() = 0;

};
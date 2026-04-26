#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

template<typename T> class RoboEyes;

class Face {
  public:
    Face();
    ~Face();
    bool Init();
    void Update();

    // Moods
    void Happy();
    void Angry();
    void Tired();
    void Neutral();

    // Looking
    void LookCenter();
    void LookUp();
    void LookDown();
    void LookLeft();
    void LookRight();

    // One-shot animations
    void Blink();
    void WinkLeft();
    void WinkRight();
    void Laugh();
    void Confused();

    // Modes
    void SetCyclops(bool on);
    void SetSweat(bool on);
    void SetCurious(bool on);

    // Aliveness
    void AutonomousMode(bool on);

    // Compound emotions
    void Suspicious();
    void Sleepy();
    void Surprised();
    void Excited();
    void Sad();
    void Bored();
    void Anxious();
    void InLove();
    void Scared();
    void Skeptical();
    void Dazed();
    void Mischievous();
    void Cute();
    void Mean();
    void Crying();
    void Sneaky();

    // Helpers used by emotions
    void SetEyeSize(int width, int height);
    void SetEyeRoundness(int radius);
    void ResetEyeShape();

  private:
    Adafruit_SSD1306 _display;
    RoboEyes<Adafruit_SSD1306>* _eyes;
    
    unsigned long _winkScheduledAt;   // for delayed winks in compound emotions
    bool _winkPending;
    bool _autonomous;
    unsigned long _moodChangeTimer;
    unsigned long _reactionTimer;
    int _currentMoodPhase;
    void _autonomousTick();
};
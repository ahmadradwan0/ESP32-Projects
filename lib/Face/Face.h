#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Forward declaration — we don't include the library here
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

  private:
    Adafruit_SSD1306 _display;
    RoboEyes<Adafruit_SSD1306>* _eyes;   // pointer instead of direct member
};
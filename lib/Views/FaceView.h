#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "IView.h"
#include "ISound.h"

// RoboEyes is a template — forward-declare so callers don't need to pull in
// the full FluxGarage header. The .cpp includes it.
template<typename T> class RoboEyes;

// FaceView — the animated robot face, expressed as an IView.
//
// Lifetime model: persistent. Constructed once at startup, Bind()-ed to the
// big OLED once during setup, then lives for the life of the program. Animation
// state (mood, blink timer, idle timer, autonomous mood phase) is preserved
// across all view switches so the face feels continuously alive.
//
// Sound is injected by reference. FaceView only calls into ISound; it does
// NOT own its lifecycle (Init/Update for the sound system happen in main.cpp,
// because the same sound system is shared across all views).
class FaceView : public IView {
  public:
    // The sound dependency is locked in at construction. The display is bound
    // separately (see Bind) because it's owned by ScreenManager.
    explicit FaceView(ISound& sound);
    ~FaceView();

    // Allocate RoboEyes against the given display. Must be called once after
    // ScreenManager::Init() has run (so the display is alive). Calling Bind
    // a second time is a no-op — FaceView is bound exactly once.
    void Bind(Adafruit_SSD1306& display);

    // ---- IView ----
    // OnEnter / OnExit intentionally use the base no-op defaults: the face
    // simply resumes where it left off when re-activated.
    void Draw(Adafruit_SSD1306& display) override;

    // ---- Moods ----
    void Happy();
    void Angry();
    void Tired();
    void Neutral();

    // ---- Looking ----
    void LookCenter();
    void LookUp();
    void LookDown();
    void LookLeft();
    void LookRight();

    // ---- One-shot animations ----
    void Blink();
    void WinkLeft();
    void WinkRight();
    void Laugh();
    void Confused();

    // ---- Effect toggles ----
    void SetCyclops(bool on);
    void SetSweat(bool on);
    void SetCurious(bool on);

    // ---- Aliveness ----
    void AutonomousMode(bool on);

    // ---- Compound emotions ----
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

    // ---- Helpers ----
    void SetEyeSize(int width, int height);
    void SetEyeRoundness(int radius);
    void ResetEyeShape();
    void EnableSound(bool on);
    void TriggerRandomEmotion();   // pick a random emotion + always sound, just once

  private:
    // Injected dependency
    ISound& _sound;
    bool    _soundEnabled;

    // Owned by FaceView, allocated in Bind()
    RoboEyes<Adafruit_SSD1306>* _eyes;

    // Wink scheduling for compound emotions that delay a wink
    unsigned long _winkScheduledAt;
    bool          _winkPending;

    // Autonomous mood drift
    bool          _autonomous;
    unsigned long _moodChangeTimer;
    unsigned long _reactionTimer;
    int           _currentMoodPhase;

    void _autonomousTick();
    void _resetEffects();
};
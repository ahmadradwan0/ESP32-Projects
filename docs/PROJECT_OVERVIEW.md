# Robot Face Project — Overview

## What it is

An animated robot "face" running on an ESP32-S3, evolving toward a fully
interactive AI-assisted talking robot. The big-picture goal is a Mochi-style
companion robot that:

- Shows expressive eyes/emotions on an OLED display
- Reacts with sound (eventually voice via I2S speaker)
- Has a hardware button for triggering interactions
- Hosts a local web control panel (phone-controllable)
- Will eventually integrate APIs (weather, news, AI) and a microphone

## Hardware

| Component | Pins / Bus | Notes |
|---|---|---|
| ESP32-S3-WROOM-1 (KeeYees dev kit) | — | Main controller |
| Big OLED, 128×64, SSD1306 | I2C bus 0 (default Wire), SDA=8, SCL=9, addr 0x3C | Face display |
| Small OLED, 128×32, SSD1306 | I2C bus 1 (own TwoWire(1)), SDA=17, SCL=18, addr 0x3C | Status display |
| Passive buzzer | GPIO 5 | Robot sounds |
| Push button | GPIO 4, INPUT_PULLUP | Triggers random emotion |
| (Pending arrival) INMP441 I2S mic | TBD | For voice input |
| (Pending arrival) MAX98357A + speaker | TBD | For voice output |

### Power
USB-C from the **UART** port on the dev board. Don't use the USB-OTG port for
flashing — it can cause enumeration issues.

### Hardware lessons learned
- **Bad USB-C cables** cause "device not enumerating" errors that look like
  the chip is dead. Try a different cable BEFORE assuming the board is broken.
- A breadboard amplifies piezo buzzer volume significantly. The buzzer will
  be much quieter when mounted in a real enclosure with foam/padding.

## Software stack

- PlatformIO with the Arduino framework
- **Critical:** `platform = espressif32@6.5.0` is pinned in `platformio.ini`.
  Newer versions have a bug where `Wire` symbols don't link properly on
  ESP32-S3. Don't bump this without testing thoroughly.
- Adafruit GFX + Adafruit SSD1306 for both OLEDs
- FluxGarage RoboEyes (GPL-3.0) for the eye animations — pulled from GitHub
  in `lib_deps`
- ArduinoJson for future API parsing
- FastLED (currently unused but kept for the onboard RGB LED option)

## Project structure (current state — refactor in progress)
TheTester/
├── platformio.ini
├── docs/
│   ├── PROJECT_OVERVIEW.md      ← this file
│   ├── ARCHITECTURE.md
│   └── PROJECT_LOG.md
├── html/
│   └── control_panel.h          ← Bootstrap-styled web UI as raw string
├── src/
│   └── main.cpp                 ← entry point; uses old Face class for now
└── lib/
├── AppServer/                ← WiFi + HTTP server wrapper (clean, keep)
├── Face/                     ← OLD monolithic class (will be deleted in Phase 2d)
├── Sound/
│   ├── Sound.h/.cpp          ← OLD concrete class (used by old Face — will be deleted)
│   ├── ISound.h              ← NEW interface
│   └── BuzzerSound.h/.cpp    ← NEW concrete impl of ISound
├── StatusScreen/             ← Owns the small OLED on its own I2C bus
├── ScreenManager/            ← NEW; owns the big OLED, hosts views
└── Views/
└── IView.h               ← Interface every view implements

There used to be `lib/UI/Screen.*` and `lib/TextScroller/`. Both unused
in the current code path. Slated for deletion when we tidy up.

## Where the project is right now

We're mid-refactor on the `feature/status-screen` branch.

- **Phase 1** (done, committed): `IView` interface + `ScreenManager` skeleton created. Old code still runs through `Face`.
- **Phase 2a** (done): Extracted `ISound` interface and created `BuzzerSound`. Both old `Sound` and new `BuzzerSound` exist.
- **Phase 2b** (next): Build `FaceView` that inherits from `IView` and uses `ISound&` (DI).
- **Phase 2c**: Switch `main.cpp` from old `Face` → new `FaceView` + `ScreenManager`.
- **Phase 2d**: Delete old `Face` and old `Sound` classes.
- **Phase 3+** (future): Internet/APIs (Open-Meteo weather first), then the AI integration when the I2S mic+speaker arrive.

## Goals beyond the refactor

- **Multi-view system**: face is the default; tapping a button or web action shows weather/news/clock/AI for ~15 seconds, then auto-returns to face
- **Sound for everything**: every view has its own jingle; eventually replaced by I2S speech
- **Microphone-driven**: clap/voice activation
- **Physical body**: 3D-printed or cardboard enclosure
- **Mood drift**: face has its own evolving inner life via autonomous mode
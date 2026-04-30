# Architecture

## Design philosophy

Everything is built for **separation of concerns** so each piece can be swapped
or extended without touching the others.

Key principles:
- **Dependency injection** for swappable hardware (sound, eventually displays)
- **Interface-based design** for things with multiple possible implementations
- **One owner per piece of hardware** — no two classes try to control the same OLED
- **Composition over big monolithic classes**
- **State changes are events**, not method calls; rendering follows state separately

## Class responsibilities

### `IView` (interface)
The contract every drawable screen "view" follows. Defines:
- `OnEnter()` — called when this view becomes active (default: no-op)
- `OnExit()` — called when leaving (default: no-op)
- `Draw(Adafruit_SSD1306&)` — pure virtual, must be implemented

Future views: `FaceView`, `WeatherView`, `NewsView`, `ClockView`, `AIView`, etc.

### `ScreenManager`
Owns the big OLED (`Adafruit_SSD1306` and its I2C bus). Manages which view
is currently drawing.

Behavior: **AUTO_RETURN** (only mode for now)
- `_defaultView` (typically the face) is always the fallback
- `ShowView(view, durationMs)` — temporarily switches to a takeover view
- After timer expires, auto-returns to default
- Replace policy: a new `ShowView` while one is active **replaces** the
  current takeover and resets the timer (last-call-wins)

Constructor takes width/height/addr/SDA/SCL/wire as configurable params
with defaults. Built to support different displays in the future.

`GetDisplay()` exposes the raw display for views that need direct access
(e.g., FaceView with RoboEyes). Most code should not use this escape hatch.

### `ISound` (interface)
Pure interface for any sound system. Methods cover emotion sounds (Happy,
Sad, etc.) and generic actions (Beep, Trill, etc.). Has `Init()`, `Update()`,
`Stop()` lifecycle hooks.

### `BuzzerSound : ISound`
Implementation using passive buzzer + Arduino `tone()` on a single GPIO.
Maintains an internal step queue for non-blocking R2-D2-style melodies.
Each "step" is either a flat tone or a frequency sweep with duration.
`Update()` advances through the queue using `millis()`.

### `FaceView : IView` (planned for Phase 2b)
Inherits from `IView`. Owns a `RoboEyes<Adafruit_SSD1306>*`. Receives
`ISound&` via constructor (DI).

Design choice (locked in): **persistent FaceView, bound once, never destroyed**.
- `Bind(Adafruit_SSD1306&)` is called once during setup
- RoboEyes is allocated against that display reference and lives forever
- Animation state preserved across all view switches
- Reasoning: simplicity + animation continuity + no heap churn

### `StatusScreen` (separate concern, not a View)
Independent from the view system entirely. Owns the small OLED on
its own `TwoWire(1)` instance / GPIO 17/18.

Stays explicit: main.cpp calls `statusScreen.ShowRequest("HAPPY")` etc.
directly inside route handlers. Did not auto-integrate with ScreenManager
because we want simple, explicit control.

### `AppServer`
Wraps WiFi + ESP32 `WebServer`. Provides typed route registration:
`OnGetHTML`, `OnGetJSON`, `OnGetText`, `OnPostText`, `OnPostJSON`.
Exposes `OnConnected(callback)` for IP notification.

### `Sound` (legacy, scheduled for deletion)
Old concrete class that pre-dates the `ISound` split. Still used by
old `Face` class. Will be deleted in Phase 2d after `Face` is gone.

### `Face` (legacy, scheduled for deletion)
Monolithic class that owns:
- Big OLED display
- RoboEyes pointer
- Sound (concrete `Sound`, not `ISound`)
- All emotion methods
- Autonomous mode state machine

Why it's being replaced: owns too many things, blocks the multi-view
architecture (display ownership conflict), tightly coupled to one sound
implementation.

## Conventions

- **Naming:** PascalCase for classes and methods. Internal/private members
  use `_underscorePrefix`. Constants use `UPPER_SNAKE_CASE`.
- **Headers:** every header starts with `#pragma once`. No include guards.
- **One class per file** generally. The file matches the class name.
- **Folder per library** under `lib/` — PlatformIO requires this.
- **Comments:** Brief intent comments are good. Don't comment what code
  obviously does.
- **Lambdas with captures** are the standard for route handlers.

## Dependency injection pattern

```cpp
// In main.cpp
BuzzerSound buzzer(5);             // concrete implementation
FaceView    face(buzzer);          // ISound& injected — auto-upcasts
WeatherView weather(buzzer);       // same buzzer, injected separately

// To swap to I2S later:
// I2SSound speaker(...);
// FaceView face(speaker);         // only this changes
```

Views never know which sound implementation they have. They take `ISound&`
and call methods on it. Reference (not pointer) is used to disallow null
and provide cleaner syntax.

## I2C buses — important quirk

ESP32-S3 has two hardware I2C controllers (0 and 1). The Arduino-ESP32
framework provides `Wire` (controller 0) globally. **`Wire1` does NOT
exist as a global** in the version we use — referring to it causes
linker errors.

To use the second I2C bus, **create your own `TwoWire(1)` instance**.
StatusScreen does this. Future code that needs a second bus should follow
the same pattern.

```cpp
TwoWire _bus = TwoWire(1);     // controller 1
_bus.begin(SDA_PIN, SCL_PIN);  // assign pins
```

## Behavior modes (future)

ScreenManager currently only does AUTO_RETURN. Easy to extend:

```cpp
enum Behavior { AUTO_RETURN, STAY_ON_SELECTED, ROTATE };
void SetBehavior(Behavior b);
```

Add when actually needed (YAGNI). Hooks for this exist mentally in the
design but not in code yet.

## Build / deploy

- `pio run` — compile
- `pio run --target upload` — flash to board
- `pio device monitor -b 115200` — serial monitor
- Always **close the monitor before uploading** or you'll get "port busy"
- After every uploaded session: `git add . && git commit && git push`

WiFi credentials are currently in `main.cpp` as plain strings. Should be
moved to a gitignored `secrets.h` before any public push. **Whenever
`main.cpp` is shared in chats, the password should be changed on the
router afterward.**
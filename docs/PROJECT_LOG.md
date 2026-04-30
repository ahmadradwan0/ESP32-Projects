# Project Log — Things tried, things that failed

A short, append-only record of significant lessons. Don't repeat these.

---

## Display

### Original `Screen` class (deleted)
First attempt at a display abstraction. Worked, but became obsolete when
we moved to ScreenManager + IView. Slated for deletion.

### Status screen on `Wire1` — failed
Tried to use the global `Wire1` for the second I2C bus. The symbol doesn't
exist in our `espressif32@6.5.0` framework. Caused cascading linker
errors. **Fix:** create our own `TwoWire(1)` instance instead. Standard
pattern now.

### `Wire` in `lib_deps` — made things worse
Trying to fix the Wire1 problem by adding `Wire` to `platformio.ini`'s
`lib_deps` actually broke the build entirely. **Don't do this.** The Wire
library comes for free with the framework — listing it explicitly confuses
the linker.

### File naming collision: `Server.h`
Originally named our HTTP server class `Server` in a file called `Server.h`.
Both the class name and the filename collided with framework internals.
Renamed to `AppServer` in `lib/AppServer/`. **Lesson:** when picking class
names, check if the framework already uses that name.

## Sound / mic

### Volume can't be controlled with resistor (much) on piezo buzzer
Tried adding 1kΩ then 10kΩ in series. Made small audible difference at
best — passive piezo buzzers are mostly "all or nothing." `tone()` PWM
duty cycle didn't help much either.
**Real fix later:** mount the buzzer in a soft enclosure (foam/padding)
or move to the I2S speaker once it arrives.

### Lower frequencies sound softer
For "robot mumble" feeling, frequencies below 700Hz feel friendly.
Above 2kHz feels like an alarm. The R2-D2 sweep style at moderate
frequencies works really well.

### CZN-15E sound sensor module — defective
A pre-built sound sensor module never responded to sound regardless of
pot adjustment, with both LEDs stuck on. Not a wiring problem — confirmed
by powering it standalone with no ESP32. Ended up setting it aside in
favor of the upcoming INMP441 I2S mic.

## Toolchain

### Avoid USB-OTG port on the dev board for flashing
Use the **UART** USB-C port. The OTG port can cause enumeration issues
during upload.

### Bad USB-C cable causes phantom "dead board" symptoms
`device descriptor read/64, error -71` repeated dozens of times in dmesg.
Symptom looks like the chip is bricked. Was just a flaky cable. **Try
a different cable before assuming hardware damage.**

### Pin the platform version
`platform = espressif32@6.5.0`. Newer versions had a regression where
Wire's `TwoWire::begin(int, int, unsigned int)` symbol wasn't linkable.
We spent hours debugging this. If you bump the platform version, test
thoroughly first.

## Architecture decisions revisited

### "Why isn't FaceView transient?"
Considered making FaceView destroyed/recreated on each entry/exit. Decided
against it because:
- Animation state (mood, blink timer, idle timer) gets wiped, breaking
  the feeling of continuity that makes the face feel alive
- Heap churn and fragmentation risk on long uptimes
- Allocation hitches mid-frame are visible
- Bind/Unbind flexibility is hypothetical, not concrete need
**Resolution:** persistent FaceView, bound once via `Bind()`, lives forever.

### Why duplicate `Sound` and `BuzzerSound` temporarily?
We're mid-refactor. Strangler pattern: new class lives alongside the old
one until the migration is complete, then old gets deleted. Confusing
in the middle but safer than ripping everything out at once.

### WiFi credentials in code
Currently in `main.cpp` as plain strings. Acceptable for hobby use, but
**always change the WiFi password on the router** after sharing
`main.cpp` in chats/screenshots. Long-term: move to gitignored
`secrets.h`. Not done yet.
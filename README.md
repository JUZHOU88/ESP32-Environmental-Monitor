# ESP32 Environmental Monitor — Embedded C++ Portfolio

**ESP32 · PlatformIO · Arduino framework · DS18B20 · GPIO · non-blocking firmware · state machine · testable C++**

A small original embedded-systems demonstration authored for a public engineering portfolio. It measures temperature using a DS18B20 sensor, reads a dry-contact door switch, and flashes an indicator LED for sensor faults, out-of-range temperatures, or a door left open. Firmware uses `millis()`-based scheduling rather than blocking `delay()` calls.

> **Educational prototype only.** Illustrative temperature limits are not clinical limits. This is not a validated medicine-storage device or an electrical safety system. No mains-voltage or relay wiring is demonstrated.

## Hardware (ESP32 DevKit example)

| Component | Connection |
| --- | --- |
| DS18B20 DATA | GPIO 4, with 4.7 kΩ pull-up to **3.3 V** |
| DS18B20 VDD / GND | 3.3 V / GND |
| Door dry-contact switch | GPIO 27 and GND (`INPUT_PULLUP`) |
| Indicator | GPIO 2: suitable low-current LED on boards that have one; otherwise use an LED with proper resistor |

**Use only low-voltage circuits.** Check the specific board pinout before connecting anything. Never apply 5 V to ESP32 GPIO. Never attach a buzzer, relay coil or mains-powered load directly to GPIO 2.

## Run on ESP32

Install [PlatformIO](https://platformio.org/) in VS Code, open this folder and select **Build** then **Upload** for `esp32dev`. Open the serial monitor at **115200 baud**. If your ESP32 board differs, update the `board` and pin definitions.

## Run real C++ core tests on a computer

The state-machine logic is independent of Arduino libraries and can be compiled using a standard C++17 compiler:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -Iinclude src/MonitorCore.cpp test/core_tests.cpp -o monitor_tests
./monitor_tests
```

Tests cover sensor faults, stale readings, temperature hysteresis, debounced door-open alarms, timer rollover and invalid configuration. GitHub Actions runs the portable tests on pushes.

## Design decisions

- **Portable core:** `MonitorCore` contains no Arduino or third-party sensor dependencies.
- **Non-blocking operation:** Uses `millis()` instead of `delay()`, allowing monitoring of multiple inputs.
- **Fault handling:** Invalid or old samples are surfaced as sensor faults.
- **Noise tolerance:** Door debounce and temperature hysteresis reduce nuisance alarms.
- **Timer rollover:** Elapsed time uses unsigned 32-bit subtraction.
- **Separation of concerns:** Device IO remains in `src/main.cpp`; logic is in `MonitorCore`.

## Limitations and next steps

- The `MonitorCore` tests are host-executable. **The physical ESP32 build and hardware behavior have not yet been verified** in this portfolio release.
- No cloud back end, database, SMS, or remote monitoring is claimed here.
- Sensor calibration, power-loss behavior, alarm hardware, enclosure, electrical and environmental safety require further design and validation for any practical deployment.
- Future work could include authenticated telemetry, local log buffering and more comprehensive hardware-in-the-loop tests.

## Author

**Justice Zhou** — Software Engineer · Embedded Systems · C#/.NET · IoT

Related: [Cold-Chain Monitoring case study](https://github.com/JUZHOU88/Cold-Chain-Monitoring-IoT-Case-Study) · [GitHub profile](https://github.com/JUZHOU88)

MIT licensed. Demonstration code is separate from commercial or client source code.

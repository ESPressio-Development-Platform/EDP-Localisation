# Building EDP-Localisation

## C++ baseline

EDP-Localisation requires C++20.

This requirement follows the ESPressio Composition baseline and is not optional for supported builds.

## Arduino-ESP32

New Arduino-ESP32 projects should prefer **PIOArduino**, which tracks a modern Arduino-ESP32/toolchain baseline suitable for C++20.

For PlatformIO-compatible project configuration, compile with:

```ini
build_unflags =
    -std=gnu++11
    -std=gnu++14
    -std=gnu++17

build_flags =
    -std=gnu++20
```

The selected Arduino-ESP32 platform/toolchain must itself support C++20. The older toolchain bundled by some historical PlatformIO Espressif32 releases is not a supported basis merely because the flag is present.

## ESP-IDF

Compile the C++ component as C++20 and expose the repository `src` directory to the component include path.

## Dependencies

Core builds require:

- EDP-System;
- EDP-Memory.

EDP-Persistence is required only when the file-backed Pack Source is instantiated.

No direct Arduino/ESP-IDF/FreeRTOS SDK include is permitted inside EDP-Localisation core source.

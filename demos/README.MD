# EDP-Localisation Demos

The runtime demonstrations are supplied for every target required by the ESPressio demo policy:

- `Arduino_IDE`;
- `PlatformIO_Arduino`;
- `PlatformIO_ESP-IDF`.

New Arduino-ESP32 PlatformIO builds use PIOArduino and C++20.

## InBinaryResolution

Demonstrates:

- immutable `InBinaryPackSource`;
- caller-supplied German requested language and British-English terminal language;
- general-string fallback;
- resolved supplying-language materialisation.

The committed `DemoGenerated.hpp` is a deterministic current-format fixture. A real application should generate its own headers/packs with `edp-localisation compile`.

## Validation

Demonstrates:

- complete pack validation;
- complete fallback-context validation;
- whole-file CRC32C detection by deliberately corrupting a copy of one pack.

Normal Resolve performs touched-data validation; this demo shows the explicit full-validation path.

## FileBackedResolution

Demonstrates the generic `FilePackSource` Composition contract and Resolver integration.

The demo intentionally supplies a tiny application-owned read-only `FileStorage` provider whose bytes are backed by the same committed demo packs. That provider advertises `ConcurrentReads`, so it truthfully satisfies the FilePackSource Need and allows the generic adapter/path/status behavior to be exercised on every demo target.

This is **not** a claim that the current `EDP-Persistence-Arduino` or `EDP-Persistence-ESP-IDF` concrete providers are compatible. Those current providers advertise `CallerSerialized` and remain intentionally rejected by FilePackSource until a separately authorised concurrency-capable provider/wrapper exists.

## Arduino IDE dependencies

Install/check out the following libraries for the Arduino IDE demos:

- EDP-System;
- EDP-Platform;
- EDP-Platform-Portable;
- EDP-Memory;
- EDP-Localisation;
- EDP-Persistence for `FileBackedResolution` only.

A C++20-capable Arduino-ESP32 toolchain is required.

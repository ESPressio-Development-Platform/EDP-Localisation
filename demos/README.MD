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

Demonstrates the generic `FilePackSource` Composition contract and Resolver integration using real Persistence concretes.

- Arduino IDE / PlatformIO Arduino use `EDP-Persistence-Arduino::FileSystemStorage` over mounted LittleFS. The demo binding profile explicitly advertises `ConcurrentReads`, creates the `localisation` directory, persists both generated EDPL fixtures through Persistence, then resolves through `FilePackSource`.
- PlatformIO ESP-IDF uses `EDP-Persistence-ESP-IDF::VfsFileStorage` over a dedicated internal-flash FAT partition. Its binding profile also explicitly advertises `ConcurrentReads`; the generated packs are written through the concrete provider before resolution.

The concrete profile defaults remain conservative. These demo profiles opt into `ConcurrentReads` specifically because the selected filesystem substrates are documented as thread-safe and the supplied Portable ByteOperations provider is stateless.

## Arduino IDE dependencies

Install/check out the following libraries for the Arduino IDE demos:

- EDP-System;
- EDP-Platform;
- EDP-Platform-Portable;
- EDP-Memory;
- EDP-Localisation;
- EDP-Persistence for `FileBackedResolution` only;
- EDP-Persistence-Arduino for the Arduino FileBackedResolution target.

The pure ESP-IDF FileBackedResolution target additionally requires EDP-Persistence-ESP-IDF and uses its supplied FAT partition table.

A C++20-capable Arduino-ESP32 toolchain is required.

# EDP-Localisation Demos

All committed demo ContractFamily fixtures use the platform-wide fixed **64-bit (8-byte) EDP Type Identifier**. Type width is not a demo/application configuration point; this keeps identities compatible across ESPressio libraries, application code, and third-party ESPressio-compatible libraries.

The runtime demonstrations are supplied for every target required by the ESPressio demo policy:

- `Arduino_IDE`;
- `PlatformIO_Arduino`;
- `PlatformIO_ESP-IDF`.

New Arduino-ESP32 PlatformIO builds use PIOArduino and C++20.

The PlatformIO projects exercise the consolidated EDP-System Composition vocabulary coherently across ESPressio dependencies. The Composition migration changed no Localisation version number.

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

Demonstrates the generic `FilePackSource` consolidated Composition `Contract` and Resolver integration using real Persistence concretes. Each target also instantiates an `Architecture` so the external Memory/Persistence Requirements are compile-time validated against the concrete provider Compositions.

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

The Arduino IDE variants select every ESPressio dependency through a public header owned by that library. Core dependencies use their public umbrellas; the portable ByteOperations concrete is selected through the narrower public `<memory/ByteOperationsProvider.hpp>` provider header so the demo does not import unrelated portable atomic/synchronization providers. FileBackedResolution explicitly includes `<ESPressio_Persistence.hpp>` before the optional Localisation Persistence surface so Arduino can discover the optional Persistence library deterministically.

Cross-library implementation headers must still enter their dependencies through public root headers; one library must not reach directly into another library's internal subpath before Arduino has discovered it.


The PlatformIO demo projects pin PIOArduino `55.03.312-1` rather than the moving `stable` alias. This release includes the SCons 4.11.1 compatibility fix required by PlatformIO Core 6.2.x. They also override `tool-esptoolpy` to PIOArduino's matching v5.4.0 `esptool.zip` source package so the platform's managed editable Python installation has the required `pyproject.toml`; this avoids the pre-compilation `No module named 'esptool'` failure seen with the default registry archive. The pure ESP-IDF FileBacked demo also pins a 4 MB sdkconfig default to match the `esp32dev` board and its custom FAT partition table. The same ESP-IDF defaults enable stack-backed FAT long filenames with a 63-character maximum so the configured filesystem can represent the `localisation` directory and `.edploc` filenames advertised by the binding profile.

## Hardware validation status

Both real-provider `FileBackedResolution` targets are validated end-to-end on representative ESP32 hardware. Arduino/LittleFS resolves the expected fallback through `FileSystemStorage`; pure ESP-IDF/FATFS resolves the same fallback through `VfsFileStorage`. A reused ESP-IDF test board carrying an older still-mountable FAT volume may require a one-time flash erase before the FAT partition is recreated under the current configuration.

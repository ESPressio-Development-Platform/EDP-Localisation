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

Core/in-binary consumers include:

```cpp
#include <ESPressio_Localisation.hpp>
```

File-backed consumers include:

```cpp
#include <ESPressio_Localisation_Persistence.hpp>
```

and must provide EDP-Persistence to the build.

The generic FilePackSource requires a selected FileStorage provider advertising at least `ConcurrentReads` and sufficient `MaximumPathBytes`. The Arduino and ESP-IDF concrete filesystem providers now expose invocation concurrency through their binding profiles. Those profiles remain `CallerSerialized` by default and should opt into `ConcurrentReads` only when that guarantee is true for the complete mounted provider configuration.

The supplied FileBacked demos use real concrete providers with explicit `ConcurrentReads` profiles. Arduino-facing FileBacked builds additionally require `EDP-Persistence-Arduino`; the pure ESP-IDF FileBacked build requires `EDP-Persistence-ESP-IDF`.

No direct Arduino/ESP-IDF/FreeRTOS SDK include is permitted inside EDP-Localisation core source.

## Host contract tests

With sibling checkouts of EDP-System, EDP-Memory and EDP-Persistence:

```bash
python3 tests/run_tests.py
```

Alternative checkout locations can be passed explicitly with `--system`, `--memory` and `--persistence`.

The host-only Python toolchain tests do not require sibling C++ repositories:

```bash
PYTHONPATH=tools python3 -m unittest discover -s tools/tests -v
```


## PlatformIO demo toolchain pin

The runtime demo PlatformIO projects pin:

```text
pioarduino/platform-espressif32 55.03.312-1
```

instead of the moving `stable` alias. This avoids the PlatformIO Core 6.2.x / older PIOArduino SCons package mismatch which can terminate ESP-IDF builds with `ModuleNotFoundError: SCons.Tool.FortranCommon`.

The demos also override `tool-esptoolpy` to PIOArduino's matching v5.4.0 source package:

```ini
platform_packages =
    tool-esptoolpy @ https://github.com/pioarduino/esptool/releases/download/v5.4.0/esptool.zip
```

PIOArduino `55.03.312-1` installs esptool into its managed Python environment with an editable package install. The platform's default registry `tool-esptoolpy` archive does not expose the Python project metadata required by that path, which can otherwise fail before application compilation with `ModuleNotFoundError: No module named 'esptool'`. The source-package override retains the same esptool 5.4.0 release while supplying its `pyproject.toml`.

The pure ESP-IDF FileBacked demo additionally commits a small `sdkconfig.defaults` containing `CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y` and declares `board_upload.flash_size = 4MB`, matching the `esp32dev` board definition and the supplied custom FAT partition table. It also enables bounded FAT long-filename support with `CONFIG_FATFS_LFN_STACK=y` and `CONFIG_FATFS_MAX_LFN=63`, matching the demo binding profile and the `localisation/*.edploc` path segments without requiring heap-backed LFN buffers.

PIOArduino generates the resolved ESP-IDF configuration as `sdkconfig.esp32dev` in the demo project directory. `sdkconfig.defaults` supplies defaults for unset values; it does not override values already settled in an existing generated sdkconfig. After pulling a change to `sdkconfig.defaults`, remove `sdkconfig.esp32dev` before rebuilding so the generated configuration is recreated from the new defaults. The FileBacked ESP-IDF demo also compile-time checks the required FAT LFN configuration so a stale generated sdkconfig fails during compilation rather than later at runtime.

# Build, Test and Source Map

C++20 is required for runtime code. `tools/` contains the Python CLI implementation. `docs/` covers architecture, Composition, EDPL format, memory/concurrency, pack sources, resolution, source contracts and tooling. `examples/getting-started` demonstrates JSON -> compile -> generated bindings/packs -> persisted ESP32 resolution.

Host contract tests require sibling EDP-System, EDP-Memory, EDP-Platform and EDP-Persistence checkouts because EDP-Memory's public contract now enters its mandatory Platform dependency through `<ESPressio_Platform.hpp>`. Alternate locations are supplied with `--system`, `--memory`, `--platform` and `--persistence`.

Arduino IDE / Arduino CLI validation selects each dependency through a public header owned by that library. The Localisation demos use the narrow public `<ESPressio_Platform_Portable_ByteOperations.hpp>` header for the portable byte provider rather than importing the complete Platform-Portable umbrella, and FileBackedResolution explicitly selects optional EDP-Persistence before the Localisation Persistence surface.

During the Primitive-introduction workstream, PlatformIO consumers pin every modified upstream repository to `feature/primitives_introduction`, including EDP-Memory, EDP-Platform-Portable, EDP-Persistence and EDP-Persistence-Arduino; unmodified upstream repositories remain pinned to `main`.

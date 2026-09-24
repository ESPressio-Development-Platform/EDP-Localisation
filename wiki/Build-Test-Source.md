# Build, Test and Source Map

C++20 is required for runtime code. `tools/` contains the Python CLI implementation. `docs/` covers architecture, Composition, EDPL format, memory/concurrency, pack sources, resolution, source contracts and tooling. `examples/getting-started` demonstrates JSON -> compile -> generated bindings/packs -> persisted ESP32 resolution.

Host contract tests require sibling EDP-System, EDP-Memory, EDP-Platform and EDP-Persistence checkouts because EDP-Memory's public contract now enters its mandatory Platform dependency through `<ESPressio_Platform.hpp>`. Alternate locations are supplied with `--system`, `--memory`, `--platform` and `--persistence`.

Arduino IDE / Arduino CLI validation selects each dependency through a public header owned by that library. The Localisation demos use the narrow public `<ESPressio_Platform_Portable_ByteOperations.hpp>` header for the portable byte provider rather than importing the complete Platform-Portable umbrella, and FileBackedResolution explicitly selects optional EDP-Persistence before the Localisation Persistence surface.

Published PlatformIO consumers resolve ESPressio dependencies from their permanent `main` branches. Temporary feature-branch pins are used only during active cross-repository development and are removed before reintegration.

The FileBacked PlatformIO ESP-IDF application explicitly includes `ESPressio_Persistence.hpp` before the optional Localisation Persistence surface so PlatformIO's dependency finder activates the separately declared EDP-Persistence library deterministically.

## Universal Type identity regression coverage

The current feature branch additionally verifies:

- schema-backed Localisation Type aliases are exactly `System::TypeIdentifier`;
- authoring rejects zero 24-bit Type Authority values;
- authoring rejects zero 40-bit authority-local Type values;
- Type/Field Resolver APIs reject invalid System Type identities with `InvalidArgument`;
- a deliberately mutated EDPL pack with a zero Type Authority and a recomputed valid whole-file CRC32C is rejected as `InvalidDataset`, proving runtime Type-schema semantic validation independently of checksum failure.

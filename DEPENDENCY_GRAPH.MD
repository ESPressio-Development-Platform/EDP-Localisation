# EDP-Localisation Dependency Graph

> Revalidated: 2026-09-23 — Composition Framework enhancement migration changes compile-time provider/requirement metadata and validation only; no ESPressio-library dependency edge changed.

> Revalidated: 2026-09-22 — Resolver/toolchain plus real Arduino and ESP-IDF FilePackSource demo integration.

## Library Dependency Tree

```text
EDP-Localisation
├── EDP-System [mandatory]
├── EDP-Memory [mandatory]
│   ├── EDP-System [transitive]
│   └── EDP-Platform [transitive]
└── EDP-Persistence [optional: required only when FilePackSource is instantiated]
    └── EDP-System [transitive]
```

## Dependency Classification

- **EDP-System — mandatory.** Supplies the Composition Framework and the compact strongly typed `FlagSet` used by Localisation result facts.
- **EDP-Memory — mandatory.** Supplies `ByteOperations`; all Localisation raw copy/move/fill/compare operations must flow through this capability.
- **EDP-Persistence — optional.** Imposed only when a consumer includes/instantiates `FilePackSource` via `ESPressio_Localisation_Persistence.hpp`. Core Resolver contracts and `InBinaryPackSource` do not require Persistence.

EDP-Localisation has no direct Arduino, ESP-IDF, FreeRTOS, filesystem, NVS, or other platform-SDK dependency.


## Demo / validation dependencies

The production library graph above is unchanged. The supplied real-provider `FileBackedResolution` demos additionally use:

- `EDP-Persistence-Arduino` **[demo/development only]** for Arduino IDE / PlatformIO Arduino, backed by LittleFS with an explicit `ConcurrentReads` binding profile;
- `EDP-Persistence-ESP-IDF` **[demo/development only]** for PlatformIO ESP-IDF, backed by a dedicated FAT partition with an explicit `ConcurrentReads` binding profile;
- `EDP-Platform-Portable` **[demo/development only]** as the stateless standard-C/C++ ByteOperations concrete.

These are not unconditional dependencies of EDP-Localisation itself.

## V2 audit revalidation

Revalidated on 2026-09-22 after the final Policy V2 compliance remediation tranche. The audit changes affect code convention, generated C++ presentation, demo result typing, and documentation only; no mandatory, optional, transitive, development, or demo dependency edge has changed.

## Composition-framework feature branch

Revalidated for `feature/composition_framework_enhancements`: the consolidated `Requirement` / `Contract` migration changes no dependency edge. EDP-System remains mandatory, EDP-Memory remains mandatory, and EDP-Persistence remains conditional on `FilePackSource`. PlatformIO validation pins matching feature branches only to validate the staged cross-repository Composition tranche coherently.

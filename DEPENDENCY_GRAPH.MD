# EDP-Localisation Dependency Graph

> Revalidated: 2026-09-22 — initial V1 implementation baseline.

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
- **EDP-Persistence — optional.** Required only by the file-backed Pack Source. Core Resolver contracts and the in-binary Pack Source do not require Persistence.

EDP-Localisation has no direct Arduino, ESP-IDF, FreeRTOS, filesystem, NVS, or other platform-SDK dependency.

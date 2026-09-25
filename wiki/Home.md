# EDP-Localisation Developer Wiki

EDP-Localisation provides deterministic, platform-neutral human-readable text resolution from generated EDPL language packs, plus the release-coupled authoring/compiler/decompiler toolchain.

This Wiki is maintained beside the code on the same repository branch. Source code and repository `docs/` remain normative; the Wiki is the internal developer explanation/navigation layer and must evolve with code changes.

## Fixed EDP Type identity

Every schema Type identity consumed by Localisation is exactly **64 bits (8 bytes)**. Every Field identity is exactly **8 bits (1 byte)** and is local to its owning Type, giving 256 possible Field identities (`0..255`) per Type. Both semantic identity Types are owned by EDP-System. Schema inventories allocate/map those identities and presentation metadata; they do not configure either width.

## Public entry point

```cpp
#include <ESPressio_Localisation.hpp>
```

## Dependencies

Mandatory core: EDP-System and EDP-Memory. EDP-Persistence is optional and only required by the Persistence-backed FilePackSource surface exposed through `ESPressio_Localisation_Persistence.hpp`.

Use [Architecture](Architecture.md), [Public API](Public-API.md), [Internal API](Internal-API.md), [Implementation](Implementation.md), [Composition](Composition.md), [Resources / Lifecycle / Concurrency](Resources-Lifecycle-Concurrency.md), and [Build / Test / Source](Build-Test-Source.md).

# EDP-Localisation Developer Wiki

EDP-Localisation provides deterministic, platform-neutral human-readable text resolution from generated EDPL language packs, plus the release-coupled authoring/compiler/decompiler toolchain.

This Wiki is maintained beside the code on the same repository branch. Source code and repository `docs/` remain normative; the Wiki is the internal developer explanation/navigation layer and must evolve with code changes.

## Fixed EDP Type identity

Every schema Type identity consumed by Localisation is exactly **64 bits (8 bytes)**. The width is fixed platform-wide so ESPressio libraries, application code, and third-party ESPressio-compatible libraries share one stable Type identity representation. Schema inventories allocate Type identities but do not configure Type width; Field identities remain Type-local and retain their schema-selected width.

## Public entry point

```cpp
#include <ESPressio_Localisation.hpp>
```

## Dependencies

Mandatory core: EDP-System and EDP-Memory. EDP-Persistence is optional and only required by the Persistence-backed FilePackSource surface exposed through `ESPressio_Localisation_Persistence.hpp`.

Use [Architecture](Architecture.md), [Public API](Public-API.md), [Internal API](Internal-API.md), [Implementation](Implementation.md), [Composition](Composition.md), [Resources / Lifecycle / Concurrency](Resources-Lifecycle-Concurrency.md), and [Build / Test / Source](Build-Test-Source.md).

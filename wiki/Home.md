# EDP-Localisation Developer Wiki

EDP-Localisation provides deterministic, platform-neutral human-readable text resolution from generated EDPL language packs, plus the release-coupled authoring/compiler/decompiler toolchain.

This Wiki is maintained beside the code on `main`. Source code and repository `docs/` remain normative; the Wiki is the internal developer explanation/navigation layer and must evolve with code changes.

## Public entry point

```cpp
#include <ESPressio_Localisation.hpp>
```

## Dependencies

Mandatory core: EDP-System and EDP-Memory. EDP-Persistence is optional and only required by the Persistence-backed FilePackSource surface exposed through `ESPressio_Localisation_Persistence.hpp`.

Use [Architecture](Architecture.md), [Public API](Public-API.md), [Internal API](Internal-API.md), [Implementation](Implementation.md), [Composition](Composition.md), [Resources / Lifecycle / Concurrency](Resources-Lifecycle-Concurrency.md), and [Build / Test / Source](Build-Test-Source.md).

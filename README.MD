# EDP-Localisation

EDP-Localisation provides deterministic, platform-neutral resolution of human-readable presentation text from externally generated language packs.

The V1 architecture intentionally keeps human-facing text out of ordinary application code where practical. Runtime code carries compact identifiers and resolves their representations from a generated `.edploc` ContractFamily.

## Current implementation status

Implementation has begun from the locked V1 architecture. The repository currently establishes:

- C++20 package/build metadata;
- mandatory EDP-System and EDP-Memory dependencies;
- Localisation Composition vocabulary;
- strongly typed identifiers and result facts;
- bounded canonical BCP47 language-identifier and context validation;
- Pack Source result/contract vocabulary;
- immutable in-binary Pack Source;
- host-side contract tests for the implemented foundation.

Resolver parsing, file-backed Pack Source, authoring/compiler tooling, demos, and complete durable documentation are being added incrementally under the tracked V1 implementation issues.

## Core design

- resolution is stateless and caller-context driven;
- caller owns output memory;
- fallback follows the explicit parent graph stored in language packs;
- an absent representation falls back, while an explicitly empty representation resolves successfully and stops fallback;
- result status and orthogonal success facts remain separate;
- multiple Resolver/Pack Source instances may coexist;
- there is no global current language and no Localisation dependency on configuration state;
- normal runtime resolution is designed to be allocation-free and bounded.

## Documentation

- [Architecture](./docs/ARCHITECTURE.MD) — ownership boundaries, runtime model, Composition relationships, and implementation status.
- [Pack Sources](./docs/PACK-SOURCES.MD) — abstract Pack Source contract and concrete source semantics.
- [Build baseline](./docs/BUILDING.MD) — C++20, PIOArduino guidance, and conventional PlatformIO requirements.

Additional focused documents will be added in the same tranches as their corresponding public facilities.

## Dependencies

```text
EDP-Localisation
├── EDP-System [mandatory]
├── EDP-Memory [mandatory]
└── EDP-Persistence [optional: only when FilePackSource is instantiated]
```

`EDP-System` supplies Composition and `FlagSet`. `EDP-Memory` supplies the mandatory `ByteOperations` capability used for raw memory operations. `EDP-Persistence` is intentionally conditional so in-binary consumers do not acquire filesystem/storage dependencies.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph.

## Tests

With sibling `EDP-System` and `EDP-Memory` checkouts present:

```bash
python3 tests/run_tests.py
```

Explicit dependency paths can be supplied with `--system` and `--memory`.

## ISR boundary

EDP-Localisation makes no ISR-safety guarantee. Resolution, pack access, validation, Persistence-backed reads, and tooling belong in ordinary execution context rather than ISR context.

## Build baseline

C++20 is mandatory. New Arduino-ESP32 consumers should prefer PIOArduino. See [Build baseline](./docs/BUILDING.MD).

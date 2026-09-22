# EDP-Localisation

EDP-Localisation provides deterministic, platform-neutral resolution of human-readable presentation text from externally generated language packs.

The V1 architecture intentionally keeps human-facing text out of ordinary application code where practical. Runtime code carries compact identifiers and resolves their representations from a generated `.edploc` ContractFamily.

## Current implementation status

The current V1 implementation now includes:

- C++20 package/build metadata;
- Localisation Composition vocabulary and strongly typed identifiers/results;
- bounded canonical BCP47 language/context validation;
- stateless Resolver fallback and caller-owned UTF-8 materialisation;
- current-release EDPL reader, touched-data validation, full validation and CRC32C;
- immutable in-binary Pack Source;
- generic optional Persistence-backed FilePackSource;
- release-coupled Python authoring/compiler/verification/resolve/decompile tooling;
- deterministic generated ContractFamily headers, packs and build manifest;
- host-side runtime/Pack Source tests and self-contained tooling regression tests.

The remaining repository-completion work is focused on policy-required demos, final documentation reconciliation, handoff/completion audit, and the external concrete Persistence concurrency prerequisite described below.

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

- [Architecture](./docs/ARCHITECTURE.MD) — ownership boundaries, runtime model, and implementation status.
- [Resolver and Fallback](./docs/RESOLUTION.MD) — lookup surfaces, fallback, representation states, output semantics and validation.
- [Pack Sources](./docs/PACK-SOURCES.MD) — abstract Pack Source contract and concrete source semantics.
- [Memory, Lifetime and Concurrency](./docs/MEMORY-CONCURRENCY.MD) — allocation boundaries, PackResource lifetime and concurrency guarantees.
- [Composition and Provider Selection](./docs/COMPOSITION.MD) — Localisation capabilities, ByteOperations dependency and optional qualified Persistence selection.
- [Authoring Source Contracts](./docs/SOURCE-CONTRACTS.MD) — manifest/catalogue/per-language JSON, schema inventories and Platform source-bundle contract.
- [EDPL Current-Release Binary Format](./docs/EDPL-FORMAT.MD) — exact current pack envelope, sections, tables, integrity and validation model.
- [Tooling](./docs/TOOLING.MD) — compilation, verification, CLI resolution, semantic recovery and determinism.
- [Build baseline](./docs/BUILDING.MD) — C++20, dependency setup, PIOArduino guidance, and conventional PlatformIO requirements.

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

With sibling `EDP-System`, `EDP-Memory`, and `EDP-Persistence` checkouts present:

```bash
python3 tests/run_tests.py
```

Explicit dependency paths can be supplied with `--system`, `--memory`, and `--persistence`.

The self-contained release-coupled tooling regression suite is:

```bash
PYTHONPATH=tools python3 -m unittest discover -s tools/tests -v
```

## ISR boundary

EDP-Localisation makes no ISR-safety guarantee. Resolution, pack access, validation, Persistence-backed reads, and tooling belong in ordinary execution context rather than ISR context.

## Build baseline

C++20 is mandatory. New Arduino-ESP32 consumers should prefer PIOArduino. See [Build baseline](./docs/BUILDING.MD).

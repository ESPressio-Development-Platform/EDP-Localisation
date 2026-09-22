# EDP-Localisation Architecture

## Purpose

EDP-Localisation externalises human-readable presentation text from application binaries and resolves that text through compact stable identities.

The library owns Localisation semantics. It does not own application language-selection policy, Persistence lifecycle, configuration state, platform SDKs, or future Serialisation wire semantics.

## Runtime ownership

The application Bootstrap owns concrete providers and their lifetimes.

Localisation uses the consolidated EDP-System Composition Framework for compile-time architectural compatibility. PackSource providers declare scoped/cardinal `Requirement` entries inside a single `Contract`, while Resolver publishes its standalone `ResolverContract`. Runtime provider instances remain ordinary C++ objects owned by Bootstrap.

The V1 runtime remains stateless with respect to selected language:

```text
caller LocalisationContext
        |
        v
Resolver
        |
        v
selected Pack Source
        |
        +--> in-binary immutable EDPL bytes
        |
        +--> optional FilePackSource -> EDP-Persistence FileStorage
```

No global/current language exists inside EDP-Localisation.

## Memory

Caller-owned output buffers are mandatory.

EDP-Localisation directly depends on EDP-Memory and routes raw byte copy/move/fill/compare operations through a Bootstrap-owned `ByteOperations` provider. The runtime design introduces no heap-owned resolved string.

Result facts use `EDP-System::FlagSet` with the smallest suitable fixed storage width.

## Pack Sources

The Pack Source abstraction is intentionally narrow:

- locate a pack by canonical BCP47 identity;
- report current resource size;
- perform exact bounded reads;
- expose the stable canonical language identity represented by a located resource.

Pack Sources do not implement fallback or Localisation lookup semantics.

The initial V1 concrete forms are:

- immutable in-binary packs;
- optional file-backed packs over EDP-Persistence.

The file-backed concrete declares an external, exact-one FileStorage Requirement advertising at least `ConcurrentReads` and sufficient `MaximumPathBytes`. The Arduino and ESP-IDF Persistence concrete filesystem adapters now expose invocation concurrency through their binding profiles. Conservative/default bindings remain `CallerSerialized`; bindings backed by a filesystem with safe concurrent reads can declare `ConcurrentReads` and satisfy FilePackSource directly.

## Result model

Mutually exclusive outcomes use dedicated strongly typed status enums.

Independent successful facts are represented by typed FlagSets. The initial Resolve facts are:

- `LanguageFallbackUsed`;
- `BufferTooSmall`.

Truncation remains a successful resolution with `BufferTooSmall` set.

## ISR boundary

EDP-Localisation is not designed as an ISR-safe facility. Ordinary resolution, validation, and storage access must be performed outside ISR context.

## Composition feature-branch status

`feature/composition_framework_enhancements` preserves the V1 Resolver/EDPL runtime behavior while migrating Localisation's architectural declarations to the consolidated Composition vocabulary. The matching EDP-System feature branch supplies `Requirement`, cardinality, `Contract`, and selection-policy primitives. No version number changes as part of this migration.

The core V1 runtime Resolver/EDPL implementation, in-binary Pack Source, generic Persistence-backed FilePackSource, and release-coupled compiler/tooling remain the behavioral baseline.

Real concrete file-backed integration is represented in the demo matrix: Arduino uses `FileSystemStorage` over LittleFS and ESP-IDF uses `VfsFileStorage` over a mounted FAT partition, both with binding profiles that explicitly advertise `ConcurrentReads`. Both real-provider paths have been built and validated end-to-end on representative ESP32 hardware. The complete three-language getting-started workflow has likewise been exercised locally through toolchain generation, PlatformIO application/filesystem builds and on-device fallback resolution. GitHub Actions remains non-authoritative for this workstream because the available Actions execution path exhausted/failed as recorded in the living handoff.

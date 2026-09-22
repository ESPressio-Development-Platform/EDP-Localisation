# EDP-Localisation Architecture

## Purpose

EDP-Localisation externalises human-readable presentation text from application binaries and resolves that text through compact stable identities.

The library owns Localisation semantics. It does not own application language-selection policy, Persistence lifecycle, configuration state, platform SDKs, or future Serialisation wire semantics.

## Runtime ownership

The application Bootstrap owns concrete providers and their lifetimes.

Localisation uses the EDP-System Composition Framework for compile-time architectural compatibility. Runtime provider instances remain ordinary C++ objects owned by Bootstrap.

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

The file-backed concrete requires a FileStorage provider advertising at least `ConcurrentReads` and sufficient `MaximumPathBytes`. Current Arduino/ESP-IDF Persistence concretes still advertise `CallerSerialized`; real-platform file-backed integration therefore remains an explicit external prerequisite until that concrete gap is closed.

## Result model

Mutually exclusive outcomes use dedicated strongly typed status enums.

Independent successful facts are represented by typed FlagSets. The initial Resolve facts are:

- `LanguageFallbackUsed`;
- `BufferTooSmall`.

Truncation remains a successful resolution with `BufferTooSmall` set.

## ISR boundary

EDP-Localisation is not designed as an ISR-safe facility. Ordinary resolution, validation, and storage access must be performed outside ISR context.

## Implementation status

This document describes the locked V1 contract. Implementation is being added tranche by tranche. Sections describing facilities not yet present in source should be treated as target contract rather than a claim that the implementation tranche is complete.

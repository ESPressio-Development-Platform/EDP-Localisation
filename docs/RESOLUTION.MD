# Resolver and Fallback Semantics

## Purpose

The EDP-Localisation Resolver is the runtime semantic owner for finding one human-readable representation from a coherent generated ContractFamily.

It is intentionally stateless with respect to language selection. Every operation receives an explicit `LocalisationContext` containing:

- RequestedLanguage;
- TerminalLanguage.

The Resolver does not read or persist a global "current language".

## Resolution surfaces

The V1 Resolver exposes distinct strongly typed operations for:

- general strings identified by DomainId + SubDomainId + StringId;
- localised display names of supported languages;
- Type Name;
- Type Description;
- Field Name;
- Field Description;
- materialising the canonical identity of a successfully resolved language handle.

Type presentation always uses the universal `EDP-System::TypeIdentifier`, with the platform-wide fixed **64-bit (8-byte)** canonical representation. The upper 24-bit Type Authority and lower 40-bit authority-local Type value must both be non-zero. Resolver Type/Field presentation methods return `InvalidArgument` immediately when supplied an invalid System Type identity rather than attempting a dataset lookup. Field identifiers are local to their owning Type and use the fixed one-byte `EDP-System::FieldIdentifier` contract. The schema inventory does not configure Field width. A ContractFamily without any Type/Field presentation universe generates the internal 0/0 absence sentinel; the corresponding identifier types are deliberately unavailable rather than manufacturing meaningless identities. The zero Type width in that sentinel means “no Type universe”, never a variable-width Type identity.

## Fallback

Fallback is an explicit persisted parent graph.

For one resolution:

1. inspect RequestedLanguage;
2. if the representation is absent, move to that pack's explicit parent;
3. continue until a representation is found or TerminalLanguage is reached.

The traversal is bounded by SupportedLanguageCount.

The Resolver treats the following as dataset failures rather than silently repairing them:

- a cycle;
- a non-terminal pack with no parent;
- a terminal-marked pack before the expected terminal language;
- the expected terminal pack not being terminal/parentless;
- a missing intermediate pack;
- a pack from a different ContractFamily.

It does not skip unavailable parent packs and jump directly to terminal.

## Representation states

Authoring state and operational failure are separate.

A representation may be:

- Absent — no representation is present; fallback continues;
- PresentEmpty — an explicit zero-length UTF-8 value is present; resolution succeeds and fallback stops;
- PresentValue — one or more UTF-8 bytes are present; resolution succeeds and fallback stops.

An unavailable pack, storage/provider failure, incompatible pack, invalid dataset or read failure is never interpreted as Absent.

## Terminal completeness

The compiler is responsible for proving the terminal ContractFamily is complete.

At runtime:

- a missing terminal general string or language display representation returns NoStringFoundForIdentifier;
- an existing terminal presentation-exposed Type/Field whose required Name representation is absent is an InvalidDataset condition;
- optional terminal Type/Field Description may remain absent and therefore resolve as NoStringFoundForIdentifier.

This distinction allows the runtime to detect a structurally impossible generated dataset without conflating it with an optional missing Description.

## Result model

Primary mutually exclusive outcomes use `LocalisationStatus`.

Independent successful facts use `LocalisationFacts` / `EDP-System::FlagSet`:

- LanguageFallbackUsed;
- BufferTooSmall.

Truncation is not a primary failure. A representation may resolve successfully while BufferTooSmall indicates that only a UTF-8-safe prefix was materialised.

On non-successful resolution:

- success facts are clear;
- BytesWritten is zero;
- RequiredBytes is zero;
- no ResolvedLanguage is returned.

## Caller-owned output

The Resolver never returns an owning string.

The caller supplies `WritableTextView` and selects:

- RawUtf8;
- NullTerminatedUtf8.

BytesWritten and RequiredBytes count UTF-8 payload bytes only. A NUL terminator, when requested, consumes capacity but is not included in either count.

A zero-capacity output is a valid size-query pattern for RawUtf8. For NullTerminatedUtf8, zero capacity cannot hold the required terminator and therefore reports BufferTooSmall even for an empty representation.

## UTF-8-safe truncation

When output capacity is too small, the Resolver writes the largest complete UTF-8 prefix that fits.

It never returns an output ending inside a multi-byte code point.

RequiredBytes still describes the full resolved payload size.

## Resolved language

Successful resolution returns the actual language pack that supplied the representation.

If that language differs from RequestedLanguage:

- LanguageFallbackUsed is set;
- ResolvedLanguage identifies the supplier.

Name and Description are independent representations. Two separate Resolve operations may legitimately obtain them from different fallback levels.

## Validation

Normal resolution performs structural validation only for data it touches. It does not scan the whole-file CRC on every lookup.

Explicit validation is provided for stronger checks:

- ValidateLanguagePack validates the complete selected pack, including CRC32C;
- ValidateContext validates the complete requested-to-terminal chain.

No hidden "validated" cache is retained, so validation remains explicit and state-free.

## Concurrency

Resolver methods are designed to be re-entrant and usable concurrently when the selected Pack Source fulfils its own concurrent-read contract.

The Resolver owns no mutable cursor, current language, scratch buffer or global lock.

See [Memory, Lifetime and Concurrency](./MEMORY-CONCURRENCY.MD) for the resource model.

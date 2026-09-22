# Pack Sources

## Purpose

A Pack Source provides immutable EDPL bytes to the Localisation Resolver without owning fallback, presentation, or language-selection semantics.

Every Pack Source exposes four operations:

- `Locate(language)`;
- `Size(resource)`;
- `Read(resource, offset, destination)`;
- `LanguageIdentity(resource)`.

The public contract is exact-read and caller-buffer based. No Pack Source exposes a mutable cursor, seek operation, native handle, runtime registry, or asynchronous interface.

## InBinaryPackSource

`InBinaryPackSource<TByteOperationsProvider>` exposes immutable EDPL byte ranges already present in application memory.

Bootstrap supplies:

- one immutable array of `InBinaryPackDescriptor` values;
- one provider satisfying `EDP-Memory::ByteOperations`.

Each descriptor contains:

- canonical `LanguageIdentifierView`;
- first immutable pack byte;
- exact pack size.

The source owns neither the descriptor array nor the pack bytes. Both must outlive the Pack Source and every resource returned by it.

`PackResource` is provider-associated and may only be obtained from successful `Locate`. It stores only a stable pointer to one descriptor.

The source performs no allocation.

## Exact-read behaviour

`Read` succeeds only when the full requested destination range lies within the immutable resource.

A zero-length read succeeds when the offset is at or before end-of-resource.

An out-of-range interval returns `PackReadStatus::OutOfRange` and transfers zero semantically consumable bytes.

All actual byte copying flows through the injected EDP-Memory ByteOperations provider.

## File-backed source

The generic Persistence-backed Pack Source remains part of V1 but is implemented in a later tranche.

Its semantic Pack Source surface is identical. The concrete storage implementation must satisfy the locked `ConcurrentReads` and path-capacity Composition requirements.

## ISR boundary

Pack Source access carries no ISR-safety guarantee.

# Memory, Lifetime and Concurrency

## Purpose

EDP-Localisation is designed for constrained systems where hidden allocation, hidden mutable caches and ambiguous provider lifetime are unacceptable.

This document describes the runtime resource contract rather than one platform-specific allocation strategy.

## Allocation model

Normal Resolver and Pack Source operations do not require heap allocation.

The runtime uses:

- caller-owned output buffers;
- fixed-size/local operation scratch;
- provider-owned persisted or immutable pack bytes;
- compact provider-associated PackResource values.

No runtime RAM index of strings, Types, Fields or languages is required.

The Python authoring toolchain is a host-only build facility and is not part of the embedded memory contract.

## Caller-owned resolved text

Resolved text is copied into caller-provided `WritableTextView`.

The Resolver does not return:

- `std::string`;
- heap-owned blobs;
- shared ownership;
- long-lived direct views into Pack Source storage.

This keeps storage-provider lifetime separate from caller text lifetime and works equally for immutable in-binary and file-backed sources.

## EDP-Memory

EDP-Localisation depends on EDP-Memory for raw memory operations.

Bootstrap supplies a provider satisfying `EDP-Memory::ByteOperations`.

Raw copy/compare work inside the library and Pack Sources is routed through that provider rather than introducing platform libc/platform-SDK assumptions in domain-neutral source.

If future implementation requires a memory primitive not exposed by EDP-Memory, EDP-Memory must not be modified implicitly; that becomes a separate authorised dependency change.

## PackResource lifetime

A PackResource is provider-associated.

It is not a universal native file handle and it is not portable across unrelated Pack Source instances.

### In-binary

The resource identifies one descriptor in the caller-owned immutable descriptor table.

The descriptor table and its referenced pack bytes must outlive:

- InBinaryPackSource;
- any resource returned by that source.

### File-backed

The resource owns only the canonical language identity inline.

It does not own:

- a native file descriptor;
- a FilePathView into operation-local memory;
- cached file size;
- a mutable cursor.

Each Size/Read operation reconstructs the canonical path using operation-local scratch.

## Concurrency

Resolver is stateless and re-entrant.

It owns no:

- mutable selected language;
- shared cursor;
- mutable scratch buffer;
- hidden validation cache;
- runtime lookup registry.

Its concurrency therefore depends on the selected Pack Source/provider meeting the required read contract.

### In-binary source

Immutable pack bytes may be consumed concurrently subject to the lifetime of the caller-owned descriptor/data storage and the supplied ByteOperations provider contract.

### FilePackSource

FilePackSource requires the selected `EDP-Persistence::FileStorage` provider to advertise:

```text
FileInvocationConcurrency >= ConcurrentReads
```

That requirement is enforced through Composition and is not relaxed at runtime.

The current Arduino/ESP-IDF EDP-Persistence concrete providers advertise CallerSerialized, so they are intentionally rejected by FilePackSource's Need today.

## Validation state

Full validation does not create a hidden "validated pack" cache.

A later Resolve still performs the required touched-data structural checks.

This avoids mutable validity state becoming a concurrency/lifetime concern.

## Stack/resource implications

FilePackSource path scratch is statically bounded by:

```text
configured pack directory bytes
+ optional '/'
+ MaximumSupportedLanguageIdentifierBytes
+ len(".edploc")
```

The selected FileStorage provider must advertise `MaximumPathBytes` at least that large.

The EDPL reader otherwise processes fixed records and small bounded scratch rather than loading a whole pack into RAM.

## ISR boundary

No ISR-safety guarantee is provided.

Do not perform Resolver, Pack Source, Persistence-backed reads or full validation in ISR context.

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

## FilePackSource

`FilePackSource<TPersistenceComposition, TPackDirectory, TLocalisationContract, TByteOperationsProvider, ...>` is the generic optional Persistence-backed Pack Source. On the Composition-framework feature branch it is a canonical `Provider<Domain, Offers<...>, Contract<...>>` whose Contract contains its external Memory and Persistence Requirements.

It is intentionally exported through `ESPressio_Localisation_Persistence.hpp`, not the core umbrella, so an in-binary consumer does not acquire an EDP-Persistence dependency merely by including EDP-Localisation.

The source selects exactly one `EDP-Persistence::FileStorage` provider from the supplied Persistence Composition. Its consolidated `PersistenceRequirement` declares:

- capability `EDP-Persistence::FileStorage`;
- scope `ExternalDomain`;
- cardinality `ExactlyProviders<1>`;
- `FileInvocationConcurrency >= ConcurrentReads`;
- `MaximumPathBytes` large enough for the configured directory, separator, maximum supported BCP47 language identity and `.edploc` suffix;
- any additional caller-supplied FileStorage constraints/Attributes.

The same Requirement is part of the provider Contract and is used for compile-time provider selection. This keeps declaration and selection semantics aligned.

The configured pack directory is a compile-time provider-relative path. An empty directory selects provider root. Pack paths are constructed algorithmically per operation as:

```text
<pack-directory>/<canonical-language>.edploc
```

or, at provider root:

```text
<canonical-language>.edploc
```

No path table or runtime registry is retained.

A successful `Locate` performs `GetFileSize` only to prove that the resource is currently addressable, then returns a compact resource owning the canonical language identity inline. The observed size is not cached as authority. `Size` performs a fresh `GetFileSize`; `Read` performs a fresh bounded `ReadFileAt`.

Persistence permits bounded/truncated successful reads. Pack Source does not: when fewer bytes are transferred than requested, `FilePackSource::Read` returns `OutOfRange` and reports the observed byte count. Resolver/parser callers therefore never consume a short read as a complete persisted record.

`EDP-Persistence-Arduino::FileSystemStorage` and `EDP-Persistence-ESP-IDF::VfsFileStorage` expose binding-level invocation concurrency. Their standard binding profiles default to `CallerSerialized`, but a mounted filesystem whose complete provider configuration genuinely supports concurrent read-side invocation may declare `ConcurrentReads`. Such a provider satisfies FilePackSource directly through its consolidated external Requirement and the enclosing Architecture validation.

This does not weaken Localisation's requirement and does not add a mutex to FilePackSource. The concurrency fact remains owned by the concrete Persistence binding.

## ISR boundary

Pack Source access carries no ISR-safety guarantee.

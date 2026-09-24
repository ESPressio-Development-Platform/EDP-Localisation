# Dependency Contracts

EDP-Localisation has mandatory core dependencies on **EDP-System** and **EDP-Memory**. **EDP-Persistence** is optional and only enters through the Persistence-backed PackSource surface.

## EDP-System

Localisation defines a shared `PackSource` capability using the EDP-System Composition Framework. Schema-backed Type presentations also use the universal `System::TypeIdentifier`; this introduces no new dependency edge because EDP-System was already mandatory.

`ResolverContract` is a standalone consumer Contract containing:

- exactly one `PackSource`, scope `AnyDomain`;
- exactly one EDP-Memory `ByteOperations`, scope `AnyDomain`.

## EDP-Memory

Every concrete PackSource provider declares `ExternalByteOperationsRequirement`: exactly one external `ByteOperations` provider.

Resolver and EDPL reader code validate the supplied ByteOperations provider contract and borrow that Bootstrap-owned provider; they do not own a byte-operation service or allocate an adapter.

## InBinaryPackSource

Offers `PackSource` and requires exactly one external ByteOperations provider. Pack descriptors and bytes remain caller owned and lifetime stable.

## Optional EDP-Persistence: FilePackSource

`FileStorageRequirement<TPackDirectory,TContract,...>` requires exactly one **external** `FileStorage` provider satisfying:

- `FileInvocationConcurrency >= ConcurrentReads`;
- `MaximumPathBytes >= required pack path bytes`;
- all additional caller-supplied constraints/attributes.

The same Requirement is used both in the provider Contract and in `TPersistenceComposition::Select<...,SelectUnique>`, preventing declaration/selection drift.

The core umbrella `ESPressio_Localisation.hpp` does not include Persistence. `ESPressio_Localisation_Persistence.hpp` owns the optional boundary.

## Package baseline

During this coordinated Primitive-introduction workstream, the feature-branch package manifest pins EDP-System to `feature/primitives_introduction` so Localisation validates against the matching universal Type-identity implementation. EDP-Memory remains on `main`. Permanent-branch pins must be restored before reintegration.

> Dependency contract audit baseline: `ad93972bf8ecd8e4418cecd7353c42226be3e0eb` (`main`).

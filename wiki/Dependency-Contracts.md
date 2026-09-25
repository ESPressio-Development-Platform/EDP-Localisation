# Dependency Contracts

EDP-Localisation has mandatory core dependencies on **EDP-System** and **EDP-Memory**. **EDP-Persistence** is optional and only enters through the Persistence-backed PackSource surface.

## EDP-System

Localisation defines a shared `PackSource` capability using the EDP-System Composition Framework. Schema-backed Type/Field presentations additionally consume the universal `System::TypeIdentifier` and `System::FieldIdentifier` contracts. Application/schema code may use System `FieldBinding`, `FieldSet`, and `SchemaType` to bind concrete C++ members to those same numeric Field identities. This introduces no new dependency edge because EDP-System was already mandatory.

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

During this active cross-repository tranche, `EDP-Localisation/feature/primitives_introduction` deliberately pins EDP-System to its matching `feature/primitives_introduction` branch so the new Field schema contract is validated coherently before reintegration. EDP-Memory remains on `main`. The System dependency will return to `main` only after EDP-System #9 is integrated.

> Field-schema dependency contract audit baseline: EDP-System feature tip `78a2d5157ffb74abd23c307a23547fa4be5d2b6b`; EDP-Localisation source/documentation tip before Wiki work `83aa04323787a3b9ce48fa452dffdd960f1358af`.

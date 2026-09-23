# Composition and Provider Selection

## Purpose

EDP-Localisation uses the EDP-System Composition Framework to express architectural compatibility at compile time while leaving runtime provider object lifetimes under application Bootstrap control.

The Composition migration on `feature/composition_framework_enhancements` uses the consolidated consumer-oriented vocabulary:

- `Requirement` describes one required capability, its scope, cardinality, and qualification constraints;
- `Contract` groups the complete requirements of one provider or standalone consumer;
- `Provider<Domain, Provides<...>, Contract<...>>` is the canonical provider declaration;
- `Architecture` proves cross-domain provider Contracts against the concrete domain Compositions selected by Bootstrap.

No runtime registry or dependency-injection container is introduced.

## Localisation domain

EDP-Localisation defines its own Composition `Domain` and a shared `PackSource` capability.

Concrete Pack Sources, including the in-binary and file-backed forms, offer that capability.

The shared capability allows more than one PackSource implementation to exist in a wider application architecture. A particular Resolver consumer nevertheless requires exactly one selected PackSource.

## Resolver consumer Contract

Resolver publishes a standalone consumer Contract:

```cpp
using ResolverContract = Framework::Contract<
    PackSourceRequirement,
    ByteOperationsRequirement
>;
```

where:

```cpp
PackSourceRequirement
    capability  = Localisation::PackSource
    scope       = AnyDomain
    cardinality = ExactlyProviders<1>

ByteOperationsRequirement
    capability  = Memory::ByteOperations
    scope       = AnyDomain
    cardinality = ExactlyProviders<1>
```

The `AnyDomain` scope is intentional because this is a standalone consumer Contract, not a provider-owned Contract.

Resolver remains templated on the concrete PackSource and ByteOperations provider Types and is bound to their Bootstrap-owned runtime objects explicitly. The Contract describes the architecture; it does not own or locate runtime instances.

## PackSource provider Contract

Both `InBinaryPackSource` and `FilePackSource` use the canonical provider form:

```cpp
Framework::Provider<
    Localisation::Domain,
    Framework::Offers<
        Framework::Offer<Localisation::PackSource>
    >,
    Framework::Contract<
        /* external requirements */
    >
>
```

Every PackSource requires exactly one external:

```text
EDP-Memory::ByteOperations
```

through `ExternalByteOperationsRequirement`:

```text
scope       = ExternalDomain
cardinality = ExactlyProviders<1>
```

Bootstrap supplies the concrete provider object used by Resolver/reader/PackSource implementations.

Localisation does not create a second memory abstraction.

## FilePackSource Persistence Requirement

`FilePackSource` is optional and adds one consolidated external `FileStorageRequirement`.

Its Requirement is:

```text
capability  = EDP-Persistence::FileStorage
scope       = ExternalDomain
cardinality = ExactlyProviders<1>

FileInvocationConcurrency >= ConcurrentReads
MaximumPathBytes >= RequiredMaximumPackPathBytes
+ caller-supplied qualification constraints / Attributes
```

The same Requirement Type is used for both:

1. the FilePackSource provider Contract; and
2. compile-time selection from the supplied Persistence Composition.

The selected provider Type is resolved through the consolidated selection surface:

```cpp
TPersistenceComposition::Select<
    PersistenceRequirement,
    Framework::SelectUnique
>
```

Before selection, FilePackSource also requires `TPersistenceComposition::SatisfiesRequirement<PersistenceRequirement>`. The Requirement's `ExactlyProviders<1>` cardinality and the explicit `SelectUnique` policy therefore agree at both validation and selection boundaries. There is no first-match-wins behavior: zero or multiple qualifying providers are compile-time errors.

## Complete application Architecture

A real file-backed application can validate all three domains together:

```cpp
using MemoryComposition = Framework::Composition<
    ESPressio::Memory::Domain,
    ByteOperations
>;

using PersistenceComposition = Framework::Composition<
    ESPressio::Persistence::Domain,
    FileStorage
>;

using LocalisationComposition = Framework::Composition<
    ESPressio::Localisation::Domain,
    PackSource
>;

using ApplicationArchitecture = Framework::Architecture<
    MemoryComposition,
    PersistenceComposition,
    LocalisationComposition
>;

static_assert(ApplicationArchitecture::IsValid);
```

This verifies that the Localisation PackSource's external Memory and Persistence requirements are satisfiable by the concrete provider set selected for the build.

The in-binary form omits the Persistence Composition and validates only Memory + Localisation.

## Optional Persistence package boundary

Core consumers include:

```cpp
#include <ESPressio_Localisation.hpp>
```

That umbrella does not include EDP-Persistence.

Consumers using FilePackSource include:

```cpp
#include <ESPressio_Localisation_Persistence.hpp>
```

and must make EDP-Persistence available to their build.

The consolidated Composition migration does not change this boundary. Persistence remains optional for in-binary deployments.

## Bootstrap ownership

Composition validates compatible provider *types*. Bootstrap still owns provider *objects*.

Typical runtime construction remains explicit:

1. Bootstrap constructs the concrete ByteOperations provider;
2. Bootstrap constructs any concrete Persistence provider;
3. Bootstrap constructs the selected PackSource with those provider instances;
4. Bootstrap constructs Resolver bound to the PackSource and ByteOperations instances;
5. callers supply LocalisationContext per operation.

There is no hidden service locator, registry lookup, allocation, or Composition-owned runtime state.

## Multiple Localisation instances

Multiple PackSource / Resolver instances may coexist.

One Resolver does not implicitly search multiple sources.

If future architecture requires ordered provider chaining, it must be represented as an explicit composite PackSource rather than hidden Resolver precedence.

## Concrete Persistence integration

The generic FilePackSource remains strict: it requires `FileInvocationConcurrency >= ConcurrentReads`.

The Arduino `FileSystemStorage` and ESP-IDF `VfsFileStorage` concrete providers advertise concurrency from their binding profile. Profiles default to `CallerSerialized`, while a binding whose mounted filesystem and injected dependencies genuinely support concurrent reads may explicitly select `ConcurrentReads`.

This preserves the original ownership boundary:

- Localisation keeps the strict qualified Requirement;
- Persistence owns the concrete concurrency guarantee;
- Composition proves compatibility;
- no runtime probing or Localisation-side lock is introduced.

## Feature-branch validation

While this migration remains unmerged, PlatformIO demos and the getting-started proof deliberately pin the matching `feature/composition_framework_enhancements` branches of their ESPressio dependencies.

No version number is changed by this migration.

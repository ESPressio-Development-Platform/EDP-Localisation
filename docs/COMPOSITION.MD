# Composition and Provider Selection

## Purpose

EDP-Localisation uses the EDP-System Composition Framework to express architectural compatibility at compile time while leaving runtime provider object lifetimes under application Bootstrap control.

## Localisation domain

EDP-Localisation defines its own Composition Domain and a shared PackSource capability.

Concrete Pack Sources, including the in-binary and file-backed forms, offer that capability.

The Resolver itself consumes a concrete Pack Source type rather than searching a global registry.

## ByteOperations dependency

Raw memory operations are an external domain dependency.

Localisation declares a cross-domain Need for:

```text
EDP-Memory::ByteOperations
```

Bootstrap supplies the concrete provider instance used by Resolver/reader/Pack Source implementations.

Localisation does not create a second memory abstraction.

## FilePackSource dependency

FilePackSource is optional and introduces a cross-domain dependency on:

```text
EDP-Persistence::FileStorage
```

Its Need includes at least:

```text
FileInvocationConcurrency >= ConcurrentReads
MaximumPathBytes >= RequiredMaximumPackPathBytes
```

Callers may append additional FileStorage constraints/Attributes to the FilePackSource specialization.

The supplied Persistence Composition resolves exactly one provider satisfying that complete Need through `ProviderSatisfying<Need>`.

Zero matches or multiple matches are compile-time errors. There is no first-match-wins or runtime probing behavior.

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

This keeps Persistence conditional rather than a mandatory dependency for in-binary deployments.

## Bootstrap ownership

Composition chooses compatible provider *types*. Bootstrap still owns provider *objects*.

Typical runtime construction therefore remains explicit:

1. Bootstrap constructs the concrete ByteOperations provider;
2. Bootstrap constructs the selected Pack Source provider and any underlying Persistence provider;
3. Bootstrap constructs the Resolver bound to those provider instances;
4. callers supply LocalisationContext per operation.

There is no hidden service locator.

## Multiple Localisation instances

Multiple Pack Source / Resolver instances may coexist.

One Resolver does not implicitly search multiple sources.

If future architecture requires ordered provider chaining, it must be represented as an explicit composite Pack Source rather than hidden Resolver precedence.

## Concrete Persistence integration

The generic FilePackSource remains strict: it requires `FileInvocationConcurrency >= ConcurrentReads`.

The Arduino `FileSystemStorage` and ESP-IDF `VfsFileStorage` concrete providers now advertise concurrency from their binding profile. Profiles default to `CallerSerialized`, while a binding whose mounted filesystem and injected dependencies genuinely support concurrent reads may explicitly select `ConcurrentReads`.

This preserves the original ownership boundary:

- Localisation keeps the strict Need;
- Persistence owns the concrete concurrency guarantee;
- Composition proves compatibility;
- no runtime probing or Localisation-side lock is introduced.

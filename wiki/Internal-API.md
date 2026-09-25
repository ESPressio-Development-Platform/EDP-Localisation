# Internal API

Internal runtime machinery parses only the EDPL structures touched by a lookup, validates offsets/ranges/records, follows the bounded fallback graph and materializes UTF-8 through injected ByteOperations.

Tooling internals own source-schema validation, canonical ordering, pack section layout, CRC32C generation/verification, manifest generation, generated-header rendering and inverse decompilation.


## Fixed Field-identity selectors

The runtime identifier layer keeps Localisation's configurable-width `NumericIdentifier` machinery only for Domain/SubDomain/String identities. Schema Field identity is selected independently: when a generated ContractFamily exposes a Type/Field universe, the internal selector yields the exact System-owned `System::FieldIdentifier`; when the universe is absent it yields the deliberately unconstructible Localisation sentinel.

This selector is internal glue, not an alternate Field identity authority. The authoritative semantic identity and concrete C++ member mapping remain owned by EDP-System.

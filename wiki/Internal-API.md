# Internal API

Internal runtime machinery parses only the EDPL structures touched by a lookup, validates offsets/ranges/records, follows the bounded fallback graph and materializes UTF-8 through injected ByteOperations.

Tooling internals own source-schema validation, canonical ordering, pack section layout, CRC32C generation/verification, manifest generation, generated-header rendering and inverse decompilation.

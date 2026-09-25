# Resources, Lifecycle and Concurrency

Runtime resolution is allocation-free at the contract boundary: callers own output buffers and PackSource storage. Resolver retains no unbounded index/cache. Fallback depth is bounded by supported-language count. FilePackSource does not add a Localisation mutex; it relies on a concrete Persistence provider truthfully advertising required read concurrency.

No ISR-safety guarantee is provided. The EDPL parser is little-endian, bounds-checked and does not map native structs over pack bytes.


## Type/Field schema resource model

The System-owned `FieldIdentifier` occupies exactly one byte. `FieldBinding`, `FieldSet`, `SchemaType`, and generic Field enumeration are compile-time metadata and add no Localisation runtime allocation, registry, mutex, task, cache, or per-instance capacity.

Localisation's runtime Field lookup key therefore carries one 8-byte TypeIdentifier plus one 1-byte FieldIdentifier. The fixed identity width does not introduce a dynamic resource dimension.

# Architecture

Runtime localisation is intentionally context-driven rather than global. Callers provide requested and terminal languages for each resolution. Resolver walks the authored fallback graph through a PackSource and writes UTF-8 into caller-owned buffers.

Pack sources are immutable byte providers. `InBinaryPackSource` uses caller-owned descriptors/bytes; `FilePackSource` selects one qualifying Persistence FileStorage provider and constructs pack paths algorithmically without a runtime registry.

The Python tooling owns strict JSON authoring contracts, deterministic EDPL compilation, generated C++ ContractFamily/identifier headers, validation, resolution inspection and decompilation.

## Schema Type and Field identity

Localisation consumes the platform-wide EDP-System schema identity contract rather than owning duplicate semantic identity Types.

- `System::TypeIdentifier` is fixed at **64 bits (8 bytes)**. Bytes 0..2 are the non-zero 24-bit Type Authority and bytes 3..7 are the non-zero 40-bit authority-local Type value.
- `System::FieldIdentifier` is fixed at **8 bits (1 byte)**. It is local to its owning Type; all values `0..255` are valid and zero is not a sentinel.

Concrete application Types associate their C++ members with Field identity through the System-owned `FieldBinding` / `FieldSet` schema. Localisation consumes the resulting `(TypeIdentifier, FieldIdentifier)` pair for Name/Description lookup; it does not own the C++ member binding.

Neither width is a per-ContractFamily or per-schema tuning parameter. A generated `0/0` Type/Field width pair means the ContractFamily has no Type/Field presentation universe. A non-empty universe is always `8/1`.

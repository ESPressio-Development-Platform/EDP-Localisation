# Architecture

Runtime localisation is intentionally context-driven rather than global. Callers provide requested and terminal languages for each resolution. Resolver walks the authored fallback graph through a PackSource and writes UTF-8 into caller-owned buffers.

Pack sources are immutable byte providers. `InBinaryPackSource` uses caller-owned descriptors/bytes; `FilePackSource` selects one qualifying Persistence FileStorage provider and constructs pack paths algorithmically without a runtime registry.

The Python tooling owns strict JSON authoring contracts, deterministic EDPL compilation, generated C++ ContractFamily/identifier headers, validation, resolution inspection and decompilation.

## Schema Type identity

Localisation consumes the platform-wide EDP Type identity contract. Every real Type identity is fixed at **64 bits (8 bytes)**; this is not a per-ContractFamily or per-schema tuning parameter. The fixed representation ensures compatibility between ESPressio libraries, applications and third-party ESPressio-compatible libraries. A generated 0/0 Type/Field width pair means the ContractFamily has no Type/Field presentation universe; it is not an alternative Type width.

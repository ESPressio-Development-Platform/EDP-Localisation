# Architecture

Runtime localisation is intentionally context-driven rather than global. Callers provide requested and terminal languages for each resolution. Resolver walks the authored fallback graph through a PackSource and writes UTF-8 into caller-owned buffers.

Pack sources are immutable byte providers. `InBinaryPackSource` uses caller-owned descriptors/bytes; `FilePackSource` selects one qualifying Persistence FileStorage provider and constructs pack paths algorithmically without a runtime registry.

The Python tooling owns strict JSON authoring contracts, deterministic EDPL compilation, generated C++ ContractFamily/identifier headers, validation, resolution inspection and decompilation.

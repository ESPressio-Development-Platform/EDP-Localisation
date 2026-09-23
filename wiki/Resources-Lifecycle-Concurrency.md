# Resources, Lifecycle and Concurrency

Runtime resolution is allocation-free at the contract boundary: callers own output buffers and PackSource storage. Resolver retains no unbounded index/cache. Fallback depth is bounded by supported-language count. FilePackSource does not add a Localisation mutex; it relies on a concrete Persistence provider truthfully advertising required read concurrency.

No ISR-safety guarantee is provided. The EDPL parser is little-endian, bounds-checked and does not map native structs over pack bytes.

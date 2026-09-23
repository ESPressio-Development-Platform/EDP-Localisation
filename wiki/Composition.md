# Composition

Localisation owns a Domain with shared PackSource capability. Resolver exposes a standalone consumer Contract requiring exactly one PackSource and one ByteOperations provider. PackSource implementations declare external ByteOperations requirements.

FilePackSource additionally requires exactly one external FileStorage provider qualified for ConcurrentReads and sufficient maximum path capacity. Composition validation and provider selection use the same Requirement so declaration and selection cannot diverge.

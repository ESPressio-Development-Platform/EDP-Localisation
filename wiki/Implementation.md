# Private Implementation

Resolver is stateless/reentrant and retains no mutable current-language, cache or cursor. Whole-file CRC validation is explicit rather than repeated for every ordinary lookup; individual lookup paths still validate touched structures.

EDPL is release-coupled rather than a permanent cross-version interchange format. Format evolution is allowed with the library/tool version, with source packs regenerated using the matching toolchain.

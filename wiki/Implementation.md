# Private Implementation

Resolver is stateless/reentrant and retains no mutable current-language, cache or cursor. Whole-file CRC validation is explicit rather than repeated for every ordinary lookup; individual lookup paths still validate touched structures.

EDPL is release-coupled rather than a permanent cross-version interchange format. Format evolution is allowed with the library/tool version, with source packs regenerated using the matching toolchain.

## Fixed Type-width enforcement

The authoring/schema layer does not accept a configurable Type identifier width. Runtime C++ uses the System-owned `TypeIdentifier`, while the Python tooling deliberately continues to manipulate the same canonical eight bytes without importing C++ object representation. Python tooling parses exactly 16 hexadecimal digits after `0x` and rejects zero 24-bit Authority or zero 40-bit authority-local components. Writers persist Type Schema records using the same 8-byte identities, and readers reject non-empty Type Schema sections that advertise any Type width other than 8 or contain an invalid System TypeIdentifier. The EDPL header retains its Type-width byte as structural self-description; `0` is reserved for the empty Type/Field-universe sentinel.

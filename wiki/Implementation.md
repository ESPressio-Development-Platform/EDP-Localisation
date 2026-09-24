# Private Implementation

Resolver is stateless/reentrant and retains no mutable current-language, cache or cursor. Whole-file CRC validation is explicit rather than repeated for every ordinary lookup; individual lookup paths still validate touched structures.

EDPL is release-coupled rather than a permanent cross-version interchange format. Format evolution is allowed with the library/tool version, with source packs regenerated using the matching toolchain.

## Fixed Type-width enforcement

The authoring/schema layer does not accept a configurable Type identifier width. Python tooling parses exactly 16 hexadecimal digits after `0x`, writers persist Type Schema records using 8-byte Type identities, and readers reject non-empty Type Schema sections that advertise any Type width other than 8. The EDPL header retains its Type-width byte as structural self-description; `0` is reserved for the empty Type/Field-universe sentinel.

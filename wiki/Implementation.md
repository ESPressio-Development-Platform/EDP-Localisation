# Private Implementation

Resolver is stateless/reentrant and retains no mutable current-language, cache or cursor. Whole-file CRC validation is explicit rather than repeated for every ordinary lookup; individual lookup paths still validate touched structures.

EDPL is release-coupled rather than a permanent cross-version interchange format. Format evolution is allowed with the library/tool version, with source packs regenerated using the matching toolchain.

## Fixed Type/Field identity enforcement

The authoring/schema layer accepts no configurable Type or Field identifier width.

Runtime C++ consumes the System-owned `TypeIdentifier` and `FieldIdentifier`. The Python tooling manipulates their canonical numeric/byte representations without importing C++ object layout:

- Type keys are exactly 16 hexadecimal digits after `0x`, with non-zero 24-bit Authority and non-zero 40-bit authority-local components;
- Field keys are canonical unsigned decimal integers in the fixed range `0..255`.

Writers persist Type Schema records using exactly 8-byte Type IDs and 1-byte Field IDs. Readers reject a non-empty Type Schema that advertises anything other than `8/1`.

The EDPL Type Schema header retains both width bytes as structural self-description and corruption/compatibility validation. `0/0` remains only the explicit no-Type/Field-universe sentinel. The EDPL format version is unchanged because this correction was made before external consumption.

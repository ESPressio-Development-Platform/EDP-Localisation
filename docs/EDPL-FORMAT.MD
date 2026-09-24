# EDPL Current-Release Binary Format

## Scope

This document describes the compiled pack format consumed by the current EDP-Localisation `0.1.0` implementation.

EDPL is release-owned. It is not a permanent multi-version compatibility surface. If a future EDP-Localisation release changes this format, language packs must be regenerated with the matching toolchain.

All persisted integers are little-endian. Native C++ struct layout is never persisted.

## Fixed preamble

The current fixed preamble is 22 bytes:

| Offset | Size | Field |
| ---: | ---: | --- |
| 0 | 4 | ASCII `EDPL` |
| 4 | 1 | FormatMajor |
| 5 | 1 | FormatMinor |
| 6 | 2 | HeaderSize |
| 8 | 1 | SectionCount |
| 9 | 1 | Flags, zero in V1 |
| 10 | 1 | DomainIdentifierBytes |
| 11 | 1 | SubDomainIdentifierBytes |
| 12 | 1 | StringIdentifierBytes |
| 13 | 1 | Reserved, zero |
| 14 | 4 | TotalFileSize |
| 18 | 4 | FileCrc32c |

The whole-file CRC32C uses the reflected Castagnoli polynomial. The four persisted CRC bytes are treated as zero while calculating the checksum.

## Section directory

Each section-directory entry is 12 bytes:

| Size | Field |
| ---: | --- |
| 1 | SectionType |
| 1 | SectionFlags, zero in V1 |
| 2 | Reserved, zero |
| 4 | absolute section offset |
| 4 | complete section length |

Current required section types are:

1. Language Metadata;
2. Language Display Names;
3. General Strings;
4. Type Schema;
5. UTF-8 Payload.

Every required section occurs exactly once. Section ranges may not overlap.

## Language Metadata

The section begins with:

```text
uint8  SectionVersion = 1
uint8  Flags            // bit 0 = TerminalDefault
uint8  LanguageTagLength
uint8  ParentTagLength
byte   ContractFamilyFingerprint[16]
byte   LanguageTag[LanguageTagLength]
byte   ParentTag[ParentTagLength]
```

Canonical BCP47 identities are ASCII and at most 255 bytes. Terminal packs have no parent. Non-terminal packs have exactly one explicit parent.

## Language Display Names

Header:

```text
uint8  SectionVersion = 1
uint8  Reserved = 0
uint16 EntryCount
```

Each entry is 16 bytes:

```text
uint32 TargetLanguageTagOffset
uint8  TargetLanguageTagLength
uint8  Reserved[3]
uint32 DisplayNameOffset
uint32 DisplayNameLength
```

Offsets reference the shared UTF-8 payload. Entries are emitted in canonical language-tag order.

## General Strings

Header:

```text
uint8  SectionVersion = 1
uint8  Flags = 0
uint16 Reserved = 0
uint32 DomainCount
uint32 SubDomainCount
uint32 StringCount
uint32 DomainTableOffset
uint32 SubDomainTableOffset
uint32 StringTableOffset
```

The section contains three sorted fixed-record tables.

Domain record:

```text
DomainIdentifier
uint32 FirstSubDomainIndex
uint32 SubDomainCount
```

SubDomain record:

```text
SubDomainIdentifier
uint32 FirstStringIndex
uint32 StringCount
```

String record:

```text
StringIdentifier
uint32 PayloadOffset
uint32 PayloadLength
```

Missing representation means no String record. A present record with zero payload length is an intentional empty representation.

## Type Schema

Header:

```text
uint8  SectionVersion = 1
uint8  Flags = 0
uint8  TypeIdentifierBytes   // 8 when Type/Field schema exists; 0 only for the empty-schema sentinel
uint8  FieldIdentifierBytes  // 1, 2, or 4 when schema exists; 0 only for the empty-schema sentinel
uint32 TypeCount
uint32 FieldCount
uint32 TypeTableOffset
uint32 FieldTableOffset
```

The Type Identifier width is not negotiated by a ContractFamily. Every EDP Type identity is exactly **64 bits (8 bytes)** so ESPressio libraries, application code, and third-party ESPressio-compatible libraries share one binary identity contract. The retained `TypeIdentifierBytes` header field is structural self-description and must contain `8` whenever this section contains a Type/Field universe. A completely absent Type/Field universe uses the internal `0/0` width sentinel.

A Type record is:

```text
TypeIdentifier[8]
uint8  RepresentationFlags
uint8  Reserved[3]
uint32 NamePayloadOffset
uint32 NamePayloadLength
uint32 DescriptionPayloadOffset
uint32 DescriptionPayloadLength
uint32 FirstFieldIndex
uint32 FieldCount
```

A Field record is:

```text
FieldIdentifier
uint8  RepresentationFlags
uint8  Reserved[3]
uint32 NamePayloadOffset
uint32 NamePayloadLength
uint32 DescriptionPayloadOffset
uint32 DescriptionPayloadLength
```

Representation flag bit 0 means Name is present. Bit 1 means Description is present. Presence plus zero length represents intentional empty text. Absent representations encode zero offset and zero length.

## UTF-8 payload

Header:

```text
uint8  SectionVersion = 1
uint8  Flags = 0
uint16 Reserved = 0
uint32 PayloadLength
```

Exactly `PayloadLength` structureless UTF-8 bytes follow.

There are no per-string headers and no persisted NUL terminators. All index offsets are relative to the first payload byte.

## Runtime validation

Ordinary Resolve performs touched-data structural validation and does not scan the complete file CRC.

`ValidateLanguagePack` performs complete validation, including:

- whole-file CRC32C;
- all section and table bounds;
- ordering and contiguous ownership ranges;
- all referenced UTF-8 representations;
- canonical language identities;
- ContractFamilyFingerprint compatibility.

The parser builds no RAM index.

# Localisation Authoring Source Contracts

## Purpose

EDP-Localisation authoring input is strict, versioned UTF-8 JSON.

The Python toolchain turns those sources, an immutable Platform Localisation Source Bundle and optional schema inventories into one coherent generated ContractFamily.

Unknown properties, duplicate object keys, malformed UTF-8/Unicode, invalid identifiers and cross-file contradictions are errors.

## Application source tree

A source root contains:

```text
manifest.json
strings_catalogue.json
<language>/
    language.json
    strings.json
    type_schema.json
```

Every language listed by the manifest must contain all three per-language files. Omission never means "empty".

The release-coupled toolchain can create the initial tree and exact Platform-bundle pin for you:

```bash
edp-localisation init \
    --source localisation \
    --platform-bundle dependencies/platform-localisation \
    --terminal-language en-GB \
    --language de \
    --language de-AT \
    --parent de-AT=de \
    --schema-inventory localisation/schema-inventory.json \
    --schema-identity MyApplication.Types
```

The scaffold is deliberately minimal and compileable. Application authors then populate the generated catalogue, per-language representations and optional schema inventory; the tool never invents application string/type semantics.

## manifest.json

Canonical V1 shape:

```json
{
  "schemaVersion": 1,
  "terminalLanguage": "en-GB",
  "supportedLanguages": ["en-GB", "de", "cs"],
  "identifierWidths": {
    "domain": 1,
    "subDomain": 1,
    "string": 2
  },
  "platformLocalisation": {
    "identity": "ESPressio-Platform-Localisation",
    "version": "1.4.0",
    "integrity": "sha256-..."
  }
}
```

The manifest owns only semantic Localisation configuration:

- authoring schema version;
- one terminal language;
- exact supported-language set;
- Application Domain/SubDomain/String widths;
- exact immutable Platform source-bundle pin.

It does not own parent relationships, Type/Field identifier widths, source paths, output paths or generated C++ namespace. Universal Type and Field identity are owned by EDP-System: every `System::TypeIdentifier` is fixed at 64 bits (8 bytes), and every Type-local `System::FieldIdentifier` is fixed at 8 bits (1 byte).

## strings_catalogue.json

The Application catalogue is the sole Application-owned authority for general-string identity, lifecycle, tooling symbols and localisation policy.

Application Domain 0 is forbidden; Platform owns Domain 0.

Ordinary StringIds begin at 3 because each active Domain/SubDomain reserves:

- 0 = Name;
- 1 = Description;
- 2 = Copyright.

Active ordinary strings declare localisation policy:

- translatable;
- canonical.

Retired identities remain reserved and must never be reassigned.

Domain, SubDomain and String symbols are tooling/generated-binding conveniences only. Numeric identities are authoritative.

Per-Domain/SubDomain metadata policy may independently mark Name, Description and Copyright as translatable or canonical.

`SubDomainId 0` is special: it is the enclosing Domain's root/static-metadata namespace, not a second independent metadata authority. Therefore:

- every active Domain must contain active `SubDomainId 0`;
- the enclosing Domain object's metadata policy governs reserved StringIds 0/1/2 for that root namespace;
- the `SubDomainId 0` catalogue entry must not repeat a separate `metadata` policy object;
- SubDomains >= 1 own their own metadata policy.

This preserves the locked uniform runtime key `DomainId + SubDomainId + StringId` without creating two source authorities for `DomainId / 0 / 0..2`.

## language.json

Example:

```json
{
  "schemaVersion": 1,
  "language": "de",
  "parent": "en-GB",
  "selfName": "Deutsch",
  "displayNames": {
    "en-GB": "Britisches Englisch"
  }
}
```

Rules:

- language is canonical BCP47 and exactly matches the directory name;
- terminal language has parent = null;
- every non-terminal language has exactly one supported parent;
- the complete graph is acyclic and every chain reaches terminal;
- selfName is mandatory, non-empty and is the sole source authority for the language's own display name;
- displayNames contains only other supported languages;
- non-terminal display names may be sparse and use fallback;
- terminal must provide a non-empty display name for every supported language.

## strings.json

Canonical V1 representation shape:

```json
{
  "schemaVersion": 1,
  "domains": {
    "1": {
      "subDomains": {
        "0": {
          "strings": {
            "0": "My Application",
            "1": "Application description"
          }
        },
        "2": {
          "strings": {
            "0": "Controls",
            "17": "Start",
            "18": "",
            "19": "Stop"
          }
        }
      }
    }
  }
}
```

Domain/SubDomain/String keys are canonical unsigned decimal strings:

- no sign;
- no whitespace;
- no leading zeroes except "0";
- values must fit the configured widths.

An absent key means Absent and permits fallback.

A present empty JSON string means PresentEmpty and stops fallback successfully.

`null` is not a V1 representation.

Canonical entries are authored only in terminal language. A non-terminal canonical representation is a compiler error.

Terminal language must provide:

- StringId 0 / Name for every active Domain/SubDomain;
- every active ordinary String representation.

Description/Copyright remain optional unless future source policy explicitly strengthens that contract.

## type_schema.json

Canonical V1 presentation shape:

```json
{
  "schemaVersion": 1,
  "types": {
    "0x0123456789ABCDEF": {
      "name": "Temperature Reading",
      "description": "A synchronised temperature observation.",
      "fields": {
        "0": {
          "name": "Temperature",
          "description": "Temperature in Degrees Celsius."
        },
        "1": {
          "name": "ReadAt"
        }
      }
    }
  }
}
```

Type keys use the platform-wide canonical **64-bit** EDP Type identity representation:

- mandatory `0x`;
- exactly 16 uppercase hexadecimal digits (8 bytes / 64 bits);
- leading zeroes required.

The fixed width, canonical byte order, and 24-bit Authority / 40-bit authority-local validity rules are an EDP-System interoperability contract, not a Localisation tuning parameter. Localisation consumes that exact `System::TypeIdentifier` representation; the Authority and local component must both be non-zero. ESPressio libraries, application code, and third-party ESPressio-compatible libraries therefore share one unambiguous Type identity.

Field keys are canonical unsigned-decimal strings in the fixed range `0..255` and are local to their Type.

For C++ schema-bearing Types, those numeric keys must be the same identities declared by the current concrete member bindings:

```cpp
struct TemperatureReading final {
    static constexpr System::TypeIdentifier Identifier = /* ... */;

    float Temperature;
    std::uint64_t RecordedAt;

    using TemperatureField = System::FieldBinding<
        &TemperatureReading::Temperature,
        0U
    >;

    using RecordedAtField = System::FieldBinding<
        &TemperatureReading::RecordedAt,
        1U
    >;

    using Fields = System::FieldSet<
        TemperatureField,
        RecordedAtField
    >;
};
```

The corresponding schema inventory must therefore use Field keys `"0"` for `Temperature` and `"1"` for `RecordedAt`. `System::FieldBinding` is the authoritative current C++ member-to-Field association; the schema inventory records the governed identity allocation, lifecycle/non-reuse history, and presentation exposure for those same identities. Localisation does not create a parallel Field identity namespace.

The authoritative schema inventory, not Localisation source, owns:

- Type identity allocation within the fixed 64-bit EDP identity universe;
- Field identity allocation and non-reuse lifecycle;
- active/retired lifecycle;
- presentation exposure.

Per-language source carries only Name/Description representations.

Terminal language requires Name for every active presentation-exposed Type and Field. Descriptions are optional. Non-terminal Name/Description representations are independently sparse.

A Type object may exist only to carry Field presentation even when the Type itself is not presentation-exposed.

## Schema inventory

The V1 interchange is strict JSON and declares:

- schemaVersion;
- fixed-width 64-bit Type identifiers;
- active/retired lifecycle;
- presentationExposed state;
- Field identifiers and equivalent lifecycle/exposure;
- optional symbols/provenance for diagnostics only.

All Type identifiers are exactly 64 bits by platform contract. All Field identifiers are exactly 8 bits by platform contract and are expressed as canonical decimal object keys in the range `0..255`. Schema inventories do not configure either width.

Duplicate TypeIdentifier across inventories is an error even if declarations appear identical.

A retired Type/Field must not be presentation-exposed. A retired Type may not contain active Fields.

Zero schema inventories are valid only when no Type/Field presentation universe exists.

## Platform Localisation Source Bundle

Platform Localisation is source-level, not a precompiled EDPL dependency.

The application manifest pins the bundle by:

- identity;
- exact immutable version;
- full SHA-256 canonical-bundle integrity.

The compiler receives an already-resolved local bundle path and verifies the pin. It performs no network resolution.

Platform exclusively owns Domain 0. Application exclusively owns Domain >= 1. General-string merge is therefore disjoint rather than override-based.

Application still owns:

- supported-language set;
- terminal language;
- parent graph.

## Deterministic compilation

Compilation validates the entire semantic input set before publication.

On success, one coherent generation emits:

- GeneratedLocalisationContract.hpp;
- GeneratedLocalisationIdentifiers.hpp;
- one pack per supported language;
- localisation-build-manifest.json.

The publication unit is the complete generated ContractFamily. Failed validation does not publish a partial replacement.

See [Tooling](./TOOLING.MD) for commands and generated-set verification.

# EDP-Localisation

EDP-Localisation provides deterministic, platform-neutral resolution of human-readable presentation text from externally generated language packs.

The V1 architecture intentionally keeps human-facing text out of ordinary application code where practical. Runtime code carries compact identifiers and resolves their representations from a generated `.edploc` ContractFamily.

## Getting started — from JSON to a running ESP32

This walkthrough deliberately starts from an empty Application localisation source tree and ends with an ESP32 resolving persisted language packs through LittleFS.

The example has:

- terminal language `en-GB`;
- German `de`, falling back to `en-GB`;
- Austrian German `de-AT`, falling back locally to `de` and then to `en-GB`;
- two Application strings: `Greeting` and `Ready`;
- one presentation-exposed Type: `TemperatureReading`;
- two presentation-exposed Fields: `Temperature` and `RecordedAt`;
- Name and Description localisation for the Type/Fields;
- a PlatformIO/PIOArduino application using real LittleFS + `EDP-Persistence-Arduino::FileSystemStorage` + `FilePackSource`.

The committed tutorial Platform bundle under [`examples/getting-started/platform-localisation`](./examples/getting-started/platform-localisation/README.MD) is deliberately minimal and exists only to make this walkthrough self-contained. Real products should use the immutable Platform Localisation Source Bundle selected by their ESPressio platform baseline.

### 1. Install the release-coupled toolchain

From an EDP-Localisation checkout:

```bash
python3 -m pip install ./tools
edp-localisation --help
```

You can also run directly from the checkout with `PYTHONPATH=tools python3 -m edp_localisation ...`.

### 2. Create the Application source skeleton

Use `init` rather than hand-creating the strict directory/file contract:

```bash
edp-localisation init \
    --source localisation \
    --platform-bundle examples/getting-started/platform-localisation \
    --terminal-language en-GB \
    --language de \
    --language de-AT \
    --parent de-AT=de \
    --schema-inventory localisation/schema-inventory.json \
    --schema-identity Example.Temperature.Types
```

The tool reads the Platform bundle identity/version, computes its canonical SHA-256 integrity, and writes the exact pin into `manifest.json`. It refuses to overwrite a non-empty source tree.

The resulting tree is:

```text
localisation/
├── manifest.json
├── strings_catalogue.json
├── schema-inventory.json
├── en-GB/
│   ├── language.json
│   ├── strings.json
│   └── type_schema.json
├── de/
│   ├── language.json
│   ├── strings.json
│   └── type_schema.json
└── de-AT/
    ├── language.json
    ├── strings.json
    └── type_schema.json
```

The initial scaffold is intentionally minimal but compileable. The next steps replace the placeholders with real Application semantics.

### 3. Declare the two Application strings

Replace `localisation/strings_catalogue.json` with:

```json
{
  "schemaVersion": 1,
  "domains": [
    {
      "id": 1,
      "symbol": "Application",
      "status": "active",
      "subDomains": [
        {
          "id": 0,
          "symbol": "Root",
          "status": "active",
          "strings": [
            {
              "id": 3,
              "symbol": "Greeting",
              "status": "active",
              "localisation": "translatable"
            },
            {
              "id": 4,
              "symbol": "Ready",
              "status": "active",
              "localisation": "translatable"
            }
          ]
        }
      ]
    }
  ]
}
```

Application Domain `0` is reserved for ESPressio Platform strings. Ordinary Application StringIds begin at `3`; `0`, `1`, and `2` are the reserved Name, Description, and Copyright identities for each Domain/SubDomain metadata namespace.

### 4. Declare the Type and its two Fields

Replace `localisation/schema-inventory.json` with:

```json
{
  "schemaVersion": 1,
  "typeIdentifierBytes": 8,
  "fieldIdentifierBytes": 2,
  "provenance": {
    "identity": "Example.Temperature.Types",
    "version": "1"
  },
  "types": {
    "0x0123456789ABCDEF": {
      "status": "active",
      "presentationExposed": true,
      "symbol": "TemperatureReading",
      "fields": {
        "0": {
          "status": "active",
          "presentationExposed": true,
          "symbol": "Temperature"
        },
        "1": {
          "status": "active",
          "presentationExposed": true,
          "symbol": "RecordedAt"
        }
      }
    }
  }
}
```

The schema inventory is the authority for Type/Field identity and exposure. The per-language `type_schema.json` files below contain only human-readable presentations.

### 5. Define the fallback graph and language names

Use these `language.json` files.

`localisation/en-GB/language.json`:

```json
{
  "schemaVersion": 1,
  "language": "en-GB",
  "parent": null,
  "selfName": "British English",
  "displayNames": {
    "de": "German",
    "de-AT": "Austrian German"
  }
}
```

`localisation/de/language.json`:

```json
{
  "schemaVersion": 1,
  "language": "de",
  "parent": "en-GB",
  "selfName": "Deutsch",
  "displayNames": {
    "en-GB": "Britisches Englisch",
    "de-AT": "Österreichisches Deutsch"
  }
}
```

`localisation/de-AT/language.json`:

```json
{
  "schemaVersion": 1,
  "language": "de-AT",
  "parent": "de",
  "selfName": "Österreichisches Deutsch",
  "displayNames": {}
}
```

The runtime fallback chain for an Austrian-German request is therefore:

```text
de-AT -> de -> en-GB
```

### 6. Populate the two strings

`localisation/en-GB/strings.json` — the terminal language must contain every required ordinary string:

```json
{
  "schemaVersion": 1,
  "domains": {
    "1": {
      "subDomains": {
        "0": {
          "strings": {
            "0": "Temperature Monitor",
            "3": "Hello",
            "4": "Ready"
          }
        }
      }
    }
  }
}
```

`localisation/de/strings.json`:

```json
{
  "schemaVersion": 1,
  "domains": {
    "1": {
      "subDomains": {
        "0": {
          "strings": {
            "0": "Temperaturmonitor",
            "3": "Hallo"
          }
        }
      }
    }
  }
}
```

`Ready` is intentionally absent from German, so it will fall through to `en-GB`.

`localisation/de-AT/strings.json`:

```json
{
  "schemaVersion": 1,
  "domains": {}
}
```

Both ordinary strings are intentionally absent from `de-AT`. Therefore:

- `Greeting` resolves from `de` as `Hallo`;
- `Ready` is absent from both `de-AT` and `de`, so it resolves from `en-GB` as `Ready`.

An absent key means “continue fallback”. A present empty string means “successfully resolve an empty value and stop fallback”.

### 7. Populate Type/Field names and descriptions

`localisation/en-GB/type_schema.json`:

```json
{
  "schemaVersion": 1,
  "types": {
    "0x0123456789ABCDEF": {
      "name": "Temperature Reading",
      "description": "A temperature reading captured by the device.",
      "fields": {
        "0": {
          "name": "Temperature",
          "description": "Temperature in degrees Celsius."
        },
        "1": {
          "name": "Recorded At",
          "description": "Time at which the reading was captured."
        }
      }
    }
  }
}
```

`localisation/de/type_schema.json`:

```json
{
  "schemaVersion": 1,
  "types": {
    "0x0123456789ABCDEF": {
      "name": "Temperaturmessung",
      "fields": {
        "0": {
          "name": "Temperatur"
        },
        "1": {
          "name": "Erfasst um"
        }
      }
    }
  }
}
```

The German descriptions are intentionally absent, so they fall back independently to British English.

`localisation/de-AT/type_schema.json`:

```json
{
  "schemaVersion": 1,
  "types": {}
}
```

For an Austrian-German request, Type/Field names therefore come from `de`, while their descriptions continue to `en-GB`.

### 8. Validate and compile the ContractFamily

First validate the complete input set without publishing output:

```bash
edp-localisation validate \
    --source localisation \
    --platform-bundle examples/getting-started/platform-localisation \
    --schema-inventory localisation/schema-inventory.json \
    --cpp-namespace Example::Localisation
```

Then compile it:

```bash
edp-localisation compile \
    --source localisation \
    --platform-bundle examples/getting-started/platform-localisation \
    --schema-inventory localisation/schema-inventory.json \
    --output generated/localisation \
    --cpp-namespace Example::Localisation
```

The generated deployment set is:

```text
generated/localisation/
├── GeneratedLocalisationContract.hpp
├── GeneratedLocalisationIdentifiers.hpp
├── localisation-build-manifest.json
└── packs/
    ├── en-GB.edploc
    ├── de.edploc
    └── de-AT.edploc
```

The generated identifier header gives consuming code strongly typed symbolic names such as:

```cpp
Example::Localisation::LocalisationIdentifiers::Application::Root::Greeting
Example::Localisation::LocalisationIdentifiers::Application::Root::Ready

Example::Localisation::LocalisationIdentifiers::Types::TemperatureReading::Type
Example::Localisation::LocalisationIdentifiers::Types::TemperatureReading::Fields::Temperature
Example::Localisation::LocalisationIdentifiers::Types::TemperatureReading::Fields::RecordedAt
```

No raw Domain/SubDomain/String/Type/Field values need to appear in ordinary consuming code.

### 9. Exercise fallback from the command line before touching the device

Verify that the published generated set is still exactly derived from the JSON sources:

```bash
edp-localisation verify-generated \
    --source localisation \
    --platform-bundle examples/getting-started/platform-localisation \
    --schema-inventory localisation/schema-inventory.json \
    --generated generated/localisation \
    --cpp-namespace Example::Localisation
```

Resolve the two strings for Austrian German:

```bash
edp-localisation resolve \
    --generated generated/localisation \
    string --language de-AT --domain 1 --subdomain 0 --string 3

edp-localisation resolve \
    --generated generated/localisation \
    string --language de-AT --domain 1 --subdomain 0 --string 4
```

The first should report `Hallo` supplied by `de`; the second should report `Ready` supplied by `en-GB`.

The Type/Field authoring can be checked the same way:

```bash
edp-localisation resolve \
    --generated generated/localisation \
    type --language de-AT --type 0x0123456789ABCDEF

edp-localisation resolve \
    --generated generated/localisation \
    field --language de-AT --type 0x0123456789ABCDEF --field 0
```

Name and Description are resolved independently, so the Type/Field Name can be supplied by `de` while its Description is supplied by `en-GB`.

### 10. Stage the generated files into a PlatformIO project

The complete tutorial PlatformIO project is committed at [`examples/getting-started/PlatformIO_Arduino`](./examples/getting-started/PlatformIO_Arduino).

Stage the generated C++ bindings and binary packs:

```bash
PROJECT=examples/getting-started/PlatformIO_Arduino

rm -rf "$PROJECT/include/generated" "$PROJECT/data/localisation"
mkdir -p "$PROJECT/include/generated" "$PROJECT/data/localisation"

cp generated/localisation/GeneratedLocalisationContract.hpp \
    "$PROJECT/include/generated/"

cp generated/localisation/GeneratedLocalisationIdentifiers.hpp \
    "$PROJECT/include/generated/"

cp generated/localisation/packs/*.edploc \
    "$PROJECT/data/localisation/"
```

PlatformIO turns `data/` into the LittleFS filesystem image. The application therefore sees:

```text
/localisation/en-GB.edploc
/localisation/de.edploc
/localisation/de-AT.edploc
```

The tutorial `platformio.ini` sets:

```ini
board_build.filesystem = littlefs
```

and uses the same pinned PIOArduino/esptool baseline as the validated EDP-Localisation demos.

### 11. Bootstrap the runtime providers

The essential bootstrap is deliberately small. First choose the concrete ByteOperations and Persistence provider:

```cpp
namespace Framework = ESPressio::System::CompositionFramework;

using Contract =
    Example::Localisation::GeneratedLocalisationContract;

using ByteOperations =
    ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

struct TutorialBinding final {};

using FileProfile =
    ESPressio::Persistence::Arduino::FileSystemBindingProfile<
        ESPressio::Persistence::RetentionLevel::Restart,
        ESPressio::Persistence::TextCaseSensitivity::CaseSensitive,
        ESPressio::Persistence::MediaRemovability::Fixed,
        127U,
        63U,
        16384ULL,
        ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
    >;

using FileStorage =
    ESPressio::Persistence::Arduino::FileSystemStorage<
        TutorialBinding,
        FileProfile,
        ByteOperations
    >;

using PersistenceComposition = Framework::Composition<
    ESPressio::Persistence::Domain,
    FileStorage
>;
```

Then bind Localisation to the persisted `localisation/` pack directory:

```cpp
inline constexpr ESPressio::Localisation::StaticPackDirectory PackDirectory{
    "localisation"
};

using PackSource = ESPressio::Localisation::FilePackSource<
    PersistenceComposition,
    PackDirectory,
    Contract,
    ByteOperations
>;

using Resolver = ESPressio::Localisation::Resolver<
    PackSource,
    ByteOperations,
    Contract
>;
```

At startup:

```cpp
LittleFS.begin(true);

ByteOperations Bytes;
FileStorage Storage(LittleFS, Bytes);
PackSource Source(Storage, Bytes);
Resolver Localisation(Source, Bytes);
```

There is deliberately no global “current language”. The caller supplies a `LocalisationContext`:

```cpp
constexpr auto AustrianGerman =
    ESPressio::Localisation::LanguageIdentifierView::Validate("de-AT");

constexpr auto BritishEnglish =
    ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");

const ESPressio::Localisation::LocalisationContext Context{
    AustrianGerman.Value,
    BritishEnglish.Value
};
```

The first language is requested; the second is the terminal fallback required by this ContractFamily.

### 12. Resolve an Application string

Use the generated symbolic identifier rather than hand-constructing numeric keys:

```cpp
namespace Ids =
    Example::Localisation::LocalisationIdentifiers;

char Text[128U]{};

const auto Result = Localisation.ResolveString(
    Context,
    Ids::Application::Root::Greeting,
    {
        Text,
        sizeof(Text)
    },
    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
);
```

For the authored example, requesting `de-AT` resolves `Greeting` to `Hallo` from `de`.

Changing only the generated identifier to:

```cpp
Ids::Application::Root::Ready
```

resolves `Ready` from `en-GB`, because both `de-AT` and `de` omit that representation.

Every call returns a logical status, success facts, byte counts, and the actual supplying-language handle; do not infer success merely from buffer contents.

### 13. Resolve the Type and Fields

The schema symbols are generated as strongly typed bindings too:

```cpp
const auto TypeName = Localisation.ResolveTypeName(
    Context,
    Ids::Types::TemperatureReading::Type,
    {
        Text,
        sizeof(Text)
    },
    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
);

const auto TemperatureName = Localisation.ResolveFieldName(
    Context,
    Ids::Types::TemperatureReading::Fields::Temperature,
    {
        Text,
        sizeof(Text)
    },
    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
);

const auto TemperatureDescription = Localisation.ResolveFieldDescription(
    Context,
    Ids::Types::TemperatureReading::Fields::Temperature,
    {
        Text,
        sizeof(Text)
    },
    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
);
```

For `de-AT`, the Type/Field names fall back to `de`; the omitted German descriptions continue independently to `en-GB`.

### 14. Build the application and filesystem, then flash both

From the tutorial PlatformIO project:

```bash
cd examples/getting-started/PlatformIO_Arduino

pio run
pio run -t buildfs

pio run -t upload
pio run -t uploadfs

pio device monitor -b 115200
```

The committed [`src/main.cpp`](./examples/getting-started/PlatformIO_Arduino/src/main.cpp) is the complete buildable form of the progressive snippets above.

Expected output includes:

```text
Greeting: Hallo  [supplied by de]
Ready: Ready  [supplied by en-GB]
Type name: Temperaturmessung  [supplied by de]
Type description: A temperature reading captured by the device.  [supplied by en-GB]
Temperature field: Temperatur  [supplied by de]
Temperature description: Temperature in degrees Celsius.  [supplied by en-GB]
RecordedAt field: Erfasst um  [supplied by de]
```

At this point the full authoring/deployment/runtime path has been exercised:

```text
JSON source
 -> edp-localisation validate/compile
 -> generated Contract + typed identifiers + .edploc packs
 -> PlatformIO LittleFS image
 -> EDP-Persistence-Arduino FileSystemStorage
 -> FilePackSource
 -> Resolver
 -> caller-owned UTF-8 output
```

The finished authoring source shown above is also committed under [`examples/getting-started/reference-source`](./examples/getting-started/reference-source). To regression-test the complete reference workflow without retyping the tutorial JSON, run:

```bash
bash examples/getting-started/prepare.sh
```

That script performs `validate -> compile -> verify-generated` and stages the resulting headers and packs into the committed PlatformIO project; it does not bypass the real toolchain.

For production use, keep generated output under build control, pin the real Platform Localisation Source Bundle, run `verify-generated` in CI, and treat a change to any source/schema/Platform input as requiring regeneration of the whole ContractFamily.

## Current implementation status

The current V1 implementation now includes:

- C++20 package/build metadata;
- Localisation Composition vocabulary and strongly typed identifiers/results;
- bounded canonical BCP47 language/context validation;
- stateless Resolver fallback and caller-owned UTF-8 materialisation;
- current-release EDPL reader, touched-data validation, full validation and CRC32C;
- immutable in-binary Pack Source;
- generic optional Persistence-backed FilePackSource;
- release-coupled Python scaffolding/authoring/compiler/verification/resolve/decompile tooling;
- deterministic generated ContractFamily headers, packs and build manifest;
- host-side runtime/Pack Source tests and self-contained tooling regression tests.

The complete V1 runtime, tooling, consumer-onboarding and representative hardware baseline was green before the final Policy V2 compliance audit. Audit Task #7 has since made non-semantic code-convention/generated-code/demo-result corrections, so the earlier green executions remain historical evidence for the unchanged architecture and behaviour but do not by themselves validate the current repository tip. Task #7 remains open until the affected host/tooling/demo/getting-started build surfaces are rerun after the audit corrections.

The previous concrete Persistence concurrency blocker has been removed. `EDP-Persistence-Arduino::FileSystemStorage` and `EDP-Persistence-ESP-IDF::VfsFileStorage` now advertise the invocation-concurrency guarantee declared by their binding profile, conservatively defaulting to `CallerSerialized`. A filesystem binding which genuinely supports safe concurrent reads can opt into `ConcurrentReads`, allowing `FilePackSource` to select the real provider without adding Localisation-side locking. The FileBacked demo matrix now uses those real concrete providers and has been validated end-to-end on representative ESP32 hardware.

## Core design

- resolution is stateless and caller-context driven;
- caller owns output memory;
- fallback follows the explicit parent graph stored in language packs;
- an absent representation falls back, while an explicitly empty representation resolves successfully and stops fallback;
- result status and orthogonal success facts remain separate;
- multiple Resolver/Pack Source instances may coexist;
- there is no global current language and no Localisation dependency on configuration state;
- normal runtime resolution is designed to be allocation-free and bounded.

## Documentation

- [Architecture](./docs/ARCHITECTURE.MD) — ownership boundaries, runtime model, and implementation status.
- [Resolver and Fallback](./docs/RESOLUTION.MD) — lookup surfaces, fallback, representation states, output semantics and validation.
- [Pack Sources](./docs/PACK-SOURCES.MD) — abstract Pack Source contract and concrete source semantics.
- [Memory, Lifetime and Concurrency](./docs/MEMORY-CONCURRENCY.MD) — allocation boundaries, PackResource lifetime and concurrency guarantees.
- [Composition and Provider Selection](./docs/COMPOSITION.MD) — Localisation capabilities, ByteOperations dependency and optional qualified Persistence selection.
- [Authoring Source Contracts](./docs/SOURCE-CONTRACTS.MD) — manifest/catalogue/per-language JSON, schema inventories and Platform source-bundle contract.
- [EDPL Current-Release Binary Format](./docs/EDPL-FORMAT.MD) — exact current pack envelope, sections, tables, integrity and validation model.
- [Tooling](./docs/TOOLING.MD) — source scaffolding, compilation, verification, CLI resolution, semantic recovery and determinism.
- [Build baseline](./docs/BUILDING.MD) — C++20, dependency setup, PIOArduino guidance, and conventional PlatformIO requirements.

## Dependencies

```text
EDP-Localisation
├── EDP-System [mandatory]
├── EDP-Memory [mandatory]
└── EDP-Persistence [optional: only when FilePackSource is instantiated]
```

`EDP-System` supplies Composition and `FlagSet`. `EDP-Memory` supplies the mandatory `ByteOperations` capability used for raw memory operations. `EDP-Persistence` is intentionally conditional so in-binary consumers do not acquire filesystem/storage dependencies.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph.

## Demos

Runtime demonstration projects are available under [demos](./demos/README.MD):

- `InBinaryResolution` — immutable packs, explicit fallback and supplying-language reporting;
- `Validation` — complete pack/context validation and CRC32C corruption rejection;
- `FileBackedResolution` — the generic Persistence-backed adapter over real concrete providers: Arduino `FileSystemStorage`/LittleFS and ESP-IDF `VfsFileStorage`/FATFS.

Each logical demo has Arduino IDE, PlatformIO Arduino/PIOArduino, and PlatformIO ESP-IDF variants. The real file-backed demos persist the generated EDPL fixtures through the concrete Persistence provider before resolving them through `FilePackSource`.

## Tests

With sibling `EDP-System`, `EDP-Memory`, and `EDP-Persistence` checkouts present:

```bash
python3 tests/run_tests.py
```

Explicit dependency paths can be supplied with `--system`, `--memory`, and `--persistence`.

The self-contained release-coupled tooling regression suite is:

```bash
PYTHONPATH=tools python3 -m unittest discover -s tools/tests -v
```

## ISR boundary

EDP-Localisation makes no ISR-safety guarantee. Resolution, pack access, validation, Persistence-backed reads, and tooling belong in ordinary execution context rather than ISR context.

## Build baseline

C++20 is mandatory. New Arduino-ESP32 consumers should prefer PIOArduino. See [Build baseline](./docs/BUILDING.MD).

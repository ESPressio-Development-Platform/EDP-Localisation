# Getting Started Example

This directory backs the consumer-facing walkthrough in the repository [README](../../README.MD).

It contains:

- `platform-localisation/` — a minimal tutorial-only Platform Localisation Source Bundle used so the authoring/compiler walkthrough is self-contained;
- `reference-source/` — the finished three-language Application authoring source produced by the walkthrough, including the schema inventory;
- `PlatformIO_Arduino/` — the complete PIOArduino/LittleFS application used at the end of the walkthrough;
- `prepare.sh` — validates, compiles and verifies `reference-source/`, then stages the generated headers and `.edploc` packs into the PlatformIO project.

The example schema uses the platform-wide fixed **64-bit (8-byte) EDP Type Identifier** and fixed **8-bit (1-byte) EDP Field Identifier**. Field identity is local to its owning Type, so each Type has 256 stable Field identities (`0..255`). Neither width is selected by the schema inventory. The committed application defines `TemperatureReading` as a real `System::SchemaType`, binds its concrete C++ members to Field identifiers `0` and `1`, compile-time checks those identities against the generated Localisation bindings, and then enumerates the complete C++ Field schema to resolve every Type/Field Name and Description.

The top-level README intentionally teaches how to build the source tree from scratch with `edp-localisation init` and then populate it by hand. The committed `reference-source/` exists as the known final state to diff against and to make regression testing the complete example straightforward.

To prepare the committed reference example in one step:

```bash
bash examples/getting-started/prepare.sh
```

Then:

```bash
cd examples/getting-started/PlatformIO_Arduino
pio run
pio run -t buildfs
pio run -t upload
pio run -t uploadfs
pio device monitor -b 115200
```

Generated/staged tutorial artefacts are ignored by Git.

# Getting Started Example

This directory backs the consumer-facing walkthrough in the repository [README](../../README.MD).

It contains:

- `platform-localisation/` — a minimal tutorial-only Platform Localisation Source Bundle used so the authoring/compiler walkthrough is self-contained;
- `reference-source/` — the finished three-language Application authoring source produced by the walkthrough, including the schema inventory;
- `PlatformIO_Arduino/` — the complete PIOArduino/LittleFS application used at the end of the walkthrough;
- `prepare.sh` — validates, compiles and verifies `reference-source/`, then stages the generated headers and `.edploc` packs into the PlatformIO project.

The example schema uses the platform-wide fixed **64-bit (8-byte) EDP Type Identifier**. The schema inventory does not select a Type width; this invariant keeps Type identity compatible across ESPressio libraries, application code, and third-party ESPressio-compatible libraries.

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

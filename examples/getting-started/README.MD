# Getting Started Example

This directory backs the consumer-facing walkthrough in the repository [README](../../README.MD).

It contains:

- `platform-localisation/` — a minimal tutorial-only Platform Localisation Source Bundle used so the authoring/compiler walkthrough is self-contained;
- `PlatformIO_Arduino/` — the complete PIOArduino/LittleFS application used at the end of the walkthrough.

The Application localisation source, generated headers and generated `.edploc` packs are intentionally **not** committed here. Follow the top-level README to:

1. create them with `edp-localisation init`;
2. populate the two strings and one two-field Type;
3. validate and compile the ContractFamily;
4. stage the generated headers under `PlatformIO_Arduino/include/generated/`;
5. stage the generated packs under `PlatformIO_Arduino/data/localisation/`;
6. build and upload both application and LittleFS image.

This keeps the example exercising the real release-coupled toolchain rather than relying on pre-generated tutorial artefacts.

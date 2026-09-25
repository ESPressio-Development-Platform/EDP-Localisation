# Compiler Definitions and Conditional Compilation

Repository build configuration supplies:

- `SDKCONFIG_DEFAULTS="sdkconfig.defaults"` — `demos/FileBackedResolution/PlatformIO_ESP-IDF/platformio.ini:11`.

Generated localisation contract data is produced by the toolchain and C++ templates/headers, not by hidden preprocessor configuration.

> Re-audited for EDP-Localisation #18 against feature tip `83aa04323787a3b9ce48fa452dffdd960f1358af`. The Field-schema work introduces no new compiler definition or conditional-compilation control.

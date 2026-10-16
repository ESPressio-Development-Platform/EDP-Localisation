# Historical Localisation archive API

`archive_generated_family(generated_root, archive_root)` stores byte-exact, verified immutable generated ContractFamily files under the canonical SHA256 semantic digest. `open_historical_family(archive_root, fingerprint, digest=None)` reuses the existing `GeneratedContractFamily` resolver after verification. These are optional offline tooling APIs, not runtime MCU consumers.

CLI commands: `archive-family` and `verify-archived-family`.

The published manifest establishes internal file integrity only; it does not authenticate the publisher, sign the schema or allocate an EDP-System TypeIdentifier. A historical toolchain migration and cross-process concurrent writer protocol are outstanding.

See [historical archive contract](../docs/HISTORICAL-CONTRACT-FAMILIES.MD).

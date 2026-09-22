#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

SOURCE="$SCRIPT_DIR/reference-source"
PLATFORM_BUNDLE="$SCRIPT_DIR/platform-localisation"
GENERATED="$SCRIPT_DIR/generated/localisation"
PROJECT="$SCRIPT_DIR/PlatformIO_Arduino"

export PYTHONPATH="$REPOSITORY_ROOT/tools"

python3 -m edp_localisation validate \
    --source "$SOURCE" \
    --platform-bundle "$PLATFORM_BUNDLE" \
    --schema-inventory "$SOURCE/schema-inventory.json" \
    --cpp-namespace Example::Localisation

python3 -m edp_localisation compile \
    --source "$SOURCE" \
    --platform-bundle "$PLATFORM_BUNDLE" \
    --schema-inventory "$SOURCE/schema-inventory.json" \
    --output "$GENERATED" \
    --cpp-namespace Example::Localisation

python3 -m edp_localisation verify-generated \
    --source "$SOURCE" \
    --platform-bundle "$PLATFORM_BUNDLE" \
    --schema-inventory "$SOURCE/schema-inventory.json" \
    --generated "$GENERATED" \
    --cpp-namespace Example::Localisation

rm -rf "$PROJECT/include/generated" "$PROJECT/data/localisation"
mkdir -p "$PROJECT/include/generated" "$PROJECT/data/localisation"

cp "$GENERATED/GeneratedLocalisationContract.hpp" \
    "$PROJECT/include/generated/"

cp "$GENERATED/GeneratedLocalisationIdentifiers.hpp" \
    "$PROJECT/include/generated/"

cp "$GENERATED"/packs/*.edploc \
    "$PROJECT/data/localisation/"

echo "Prepared: $PROJECT"
echo "Next:"
echo "  cd $PROJECT"
echo "  pio run"
echo "  pio run -t buildfs"
echo "  pio run -t upload"
echo "  pio run -t uploadfs"
echo "  pio device monitor -b 115200"

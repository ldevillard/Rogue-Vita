#!/usr/bin/env bash

set -Eeuo pipefail

readonly GREEN="\033[1;32m"
readonly YELLOW="\033[1;33m"
readonly RED="\033[1;31m"
readonly RESET="\033[0m"

function print_usage()
{
    echo "Usage: $0 <vpk-file> <title-id> <vita3k-filesystem> <vita3k-executable>"
    echo
    echo "Example:"
    echo "  $0 rogue-vita.vpk VSDK00007 \\"
    echo "    /mnt/c/Users/logan/AppData/Roaming/Vita3K/Vita3K \\"
    echo "    /mnt/c/Users/logan/Documents/Vita-Emul-4056/Vita3K.exe"
}

function fail()
{
    echo -e "${RED}Error: $1${RESET}" >&2
    exit 1
}

if [[ $# -ne 4 ]]; then
    print_usage
    exit 1
fi

readonly VPK_PATH="$1"
readonly TITLE_ID="$2"
readonly VITA3K_FS="$3"
readonly VITA3K_EXE="$4"

readonly APP_ROOT="${VITA3K_FS}/ux0/app"
readonly APP_DIR="${APP_ROOT}/${TITLE_ID}"
TEMP_DIR=""

function cleanup()
{
    if [[ -n "$TEMP_DIR" && -d "$TEMP_DIR" ]]; then
        rm -rf "$TEMP_DIR"
    fi
}

trap cleanup EXIT

command -v unzip >/dev/null 2>&1 ||
    fail "'unzip' is not installed."

command -v wslpath >/dev/null 2>&1 ||
    fail "'wslpath' is unavailable."

[[ -f "$VPK_PATH" ]] ||
    fail "VPK file not found: $VPK_PATH"

[[ -d "$VITA3K_FS" ]] ||
    fail "Vita3K filesystem not found: $VITA3K_FS"

[[ -f "$VITA3K_EXE" ]] ||
    fail "Vita3K executable not found: $VITA3K_EXE"

[[ -d "$APP_DIR" ]] ||
    fail "The application is not installed in Vita3K. Install it once before using this script."

echo -e "${YELLOW}Stopping Vita3K...${RESET}"
taskkill.exe /IM Vita3K.exe /F >/dev/null 2>&1 || true

mkdir -p "$APP_ROOT"
TEMP_DIR="$(mktemp -d "${APP_ROOT}/${TITLE_ID}.deploy.XXXXXX")"

echo -e "${YELLOW}Extracting ${VPK_PATH}...${RESET}"
unzip -q "$VPK_PATH" -d "$TEMP_DIR"

[[ -f "${TEMP_DIR}/eboot.bin" ]] ||
    fail "The VPK does not contain eboot.bin."

[[ -f "${TEMP_DIR}/sce_sys/param.sfo" ]] ||
    fail "The VPK does not contain sce_sys/param.sfo."

echo -e "${YELLOW}Updating ${TITLE_ID}...${RESET}"
rm -rf "$APP_DIR"
mv "$TEMP_DIR" "$APP_DIR"
TEMP_DIR=""

readonly VITA3K_EXE_WINDOWS="$(wslpath -w "$VITA3K_EXE")"

echo -e "${YELLOW}Launching ${TITLE_ID}...${RESET}"
powershell.exe -NoProfile -Command \
    "Start-Process -FilePath '${VITA3K_EXE_WINDOWS}' -ArgumentList '-r','${TITLE_ID}'"

echo -e "${GREEN}Deployment completed successfully.${RESET}"
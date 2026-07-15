#!/usr/bin/env bash

set -e

: "${VITA_IP:?Missing VITA_IP}"
: "${TITLE_ID:?Missing TITLE_ID}"
: "${EBOOT_PATH:?Missing EBOOT_PATH}"

echo "[Vita] Stopping $TITLE_ID..."
printf "kill %s\n" "$TITLE_ID" | nc -w 1 "$VITA_IP" 1338 || true

echo "[Vita] Uploading eboot.bin..."
curl --fail --silent --show-error \
    -T "$EBOOT_PATH" \
    "ftp://$VITA_IP:1337//ux0:/app/$TITLE_ID/eboot.bin"

echo "[Vita] Launching $TITLE_ID..."
printf "launch %s\n" "$TITLE_ID" | nc -w 1 "$VITA_IP" 1338

echo "[Vita] Done."

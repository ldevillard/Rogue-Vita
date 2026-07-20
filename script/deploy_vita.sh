#!/usr/bin/env bash

set -e

: "${VITA_IP:?Missing VITA_IP}"
: "${TITLE_ID:?Missing TITLE_ID}"
: "${EBOOT_PATH:?Missing EBOOT_PATH}"
: "${ASSET_PATH:?Missing ASSET_PATH}"

upload_directory()
{
    local source_directory="$1"
    local destination_directory="$2"

    while IFS= read -r -d '' file; do
        local relative_path="${file#"$source_directory"/}"

        echo "[Vita] Uploading $destination_directory/$relative_path..."
        curl --fail --silent --show-error --ftp-create-dirs \
            -T "$file" \
            "ftp://$VITA_IP:1337//ux0:/app/$TITLE_ID/$destination_directory/$relative_path"
    done < <(find "$source_directory" -type f -print0)
}

echo "[Vita] Stopping $TITLE_ID..."
printf "kill %s\n" "$TITLE_ID" | nc -w 1 "$VITA_IP" 1338 || true

echo "[Vita] Uploading eboot.bin..."
curl --fail --silent --show-error \
    -T "$EBOOT_PATH" \
    "ftp://$VITA_IP:1337//ux0:/app/$TITLE_ID/eboot.bin"

upload_directory "$ASSET_PATH/shader" "asset/shaders"
upload_directory "$ASSET_PATH/cooked" "asset/cooked"

echo "[Vita] Launching $TITLE_ID..."
printf "launch %s\n" "$TITLE_ID" | nc -w 1 "$VITA_IP" 1338

echo "[Vita] Done."

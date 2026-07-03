#!/usr/bin/env bash

FILE="$1"
DEST="$2"

GREEN="\033[1;32m"
RED="\033[1;31m"
NC="\033[0m"

if [ -z "$FILE" ] || [ -z "$DEST" ]; then
    echo -e "${RED}Usage: ./deploy_emul <file> <dest>${NC}"
    exit 1
fi

if cp "$FILE" "$DEST"; then
    echo -e "${GREEN}Deploy success: $FILE -> $DEST${NC}"
else
    echo -e "${RED}Deploy failed: $FILE -> $DEST${NC}"
    exit 1
fi
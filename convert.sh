#!/bin/bash
find ./audio -type f -name "*.mp3" -print0 | while IFS= read -r -d '' f; do
    out="${f%.mp3}__tmp.mp3"
    ./ffmpeg.exe -i "$f" -t 30 -ac 1 -b:a 64k -y "$out" -loglevel error
    if [ -f "$out" ]; then
        mv -f "$out" "$f"
        echo "OK: $f"
    else
        echo "ECHEC: $f"
    fi
done
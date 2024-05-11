#!/bin/bash

f="$1"

chmod 777 "$f"

LINES=$(wc -l < "$f")
WORDS=$(wc -w < "$f")
CHARS=$(wc -c < "$f")

if grep -q -P '[^\x00-\x7F]' "$f" || \
   grep -q -E 'malware|dangerous|risk|attack' "$f" || \
   (( LINES < 3 && CHARS > 2000 && WORDS > 1000)); then
    echo $f
else
    echo "SAFE"
fi
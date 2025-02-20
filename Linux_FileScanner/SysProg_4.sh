#!/bin/bash
#Le Phan Huu Nghia

echo "input file name"
read FileName
echo "input parent directory path"
read Path

echo "-------Full Path---------"
results=$(find "$Path" -type f -iname "$FileName")
for result in $results; do
    echo "$result"
    echo ""
done

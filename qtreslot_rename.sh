#!/bin/sh

find . -name '*.qtreslot' | while read file; do
    dir=$(dirname "$file")
    name=$(basename "$file" .qtreslot)
    mv "$file" "$dir/$name"
done

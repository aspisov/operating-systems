#!/bin/bash
# This script lists files in the current directory until a specific file is found.

echo "Enter the name of the file you are looking for:"
read target_file

echo "Looking for $target_file in the current directory..."
for file in $(ls -1); do
    echo "Found file: $file"
    if [ "$file" == "$target_file" ]; then
        echo "$target_file has been found."
        break
    fi
done

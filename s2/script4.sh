#!/bin/bash
# This script counts the number of directories within the current directory.

count=0 # Counter for the directories

for dir in */; do
    if [ -d "$dir" ]; then
        count=$((count + 1)) # Increment the counter for each directory
    fi
done

echo "There are $count directories in the current directory."

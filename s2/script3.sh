#!/bin/bash
# This script waits for a specified directory to be empty.

echo "Enter the directory you want to monitor for emptiness:"
read dir

echo "Waiting for $dir to be empty..."
while [ "$(ls -A "$dir")" ]; do
    echo "$dir is not empty yet."
    sleep 5 # Wait for 5 seconds before checking again
done
echo "$dir is now empty."
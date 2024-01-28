#!/bin/bash
# This script checks if a specified directory exists.

echo "Enter the directory you want to check:"
read dir

if [ -d "$dir" ]; then
    echo "The directory $dir exists."
else
    echo "The directory $dir does not exist."
fi
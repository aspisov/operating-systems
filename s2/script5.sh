#!/bin/bash
# This script prints out line numbers before each line of a text file.

echo "Enter the name of the file to print with line numbers:"
read file

line_number=1 # Line number counter

while IFS= read -r line; do
    echo "$line_number: $line" # Print the line number and the line of text
    line_number=$((line_number + 1)) # Increment the line number
done < "$file"

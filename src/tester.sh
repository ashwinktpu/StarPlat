#!/bin/sh
# This script runs the executable (finalcode) on all possible test cases.
for filename in ../graphcode/*; do
    echo $filename
    cat "$filename" | ./finalcode
done
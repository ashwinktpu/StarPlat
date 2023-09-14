#!/bin/bash

hipcc ../bc_dsl.cc -o bc

for file in /persistent/unweighted/*.txt; do

  echo "\nRunning BC on $file with $1\n"
  ./bc $file $1
  printf "Completed\n\n\n"

done

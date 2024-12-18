#!/bin/bash
# Reformat all files.
# Author: Alex Chiang
# Usage: ./reformat.sh

set -e

repo_root_path=$(git rev-parse --show-toplevel)
types=("*.c" "*.cc" "*.cpp" "*.h" "*.hpp")

for type in "${types[@]}"
do
  find $repo_root_path -name "$type" -print0 | while IFS= read -r -d '' file
  do
    clang-format -i -style=file "$file"
  done
done

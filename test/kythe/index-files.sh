#!/bin/bash

# kill any child processes when this script exits
# trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

set -euo pipefail

output_path=/tmp/kythe-serving
rm -rf "$output_path"

entries_path=$(mktemp)

default_input_files=(test/testdata/basic.rb test/testdata/basic2.rb)
# use args as input, but fall back to the default array with 2 files
input_files=("${@:-${default_input_files[@]}}")

bazel build //main:sorbet

bazel-bin/main/sorbet --print=kythe-json --silence-dev-message "${input_files[@]}" \
  | entrystream --read_format=json >"$entries_path"

write_tables --experimental_beam_pipeline --entries "$entries_path" --out "$output_path"

echo "done writing to $output_path"

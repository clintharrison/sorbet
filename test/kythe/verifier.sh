#!/usr/bin/env bash

set -eo pipefail

# TODO: don't use this path lol
KYTHE_TOOLS_BASE="${HOME}/.kythe_binaries/kythe-v0.0.52/tools"
ENTRYSTREAM="${KYTHE_TOOLS_BASE}/entrystream"
VERIFIER="${KYTHE_TOOLS_BASE}/verifier"

# build sorbet :^)
./bazel build //main:sorbet -j 16

TEST_FILE="test/testdata/basic.rb"

# run sorbet on the test input
bazel-bin/main/sorbet \
    --silence-dev-message \
    --no-error-count \
    --print kythe-json \
    --experimental-kythe-corpus testdata \
    "${TEST_FILE}" \
| "${ENTRYSTREAM}" --read_format=json \
| "${VERIFIER}" --goal_prefix "#-" --nofile_vnames "$@" "${TEST_FILE}"
#!/usr/bin/env bash

set -eo pipefail

ENTRYSTREAM="$HOME/stripe/kythe/bazel-bin/kythe/go/platform/tools/entrystream/entrystream"
VERIFIER="$HOME/stripe/kythe/bazel-bin/kythe/cxx/verifier/verifier"

# build sorbet :^)
./bazel build //kythe:sorbet -j 16

TEST_FILE="test/testdata/basic.rb"

# run sorbet on the test input
bazel-bin/main/sorbet \
    --silence-dev-message \
    --no-error-count \
    # --experimental-kythe-corpus testdata \
    "${TEST_FILE}" \
# | "${ENTRYSTREAM}" --read_format=json \
# | "${VERIFIER}" --goal_prefix "#-" --nofile_vnames "$@" "${TEST_FILE}"

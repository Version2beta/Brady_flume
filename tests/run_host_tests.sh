#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/brady-flume-tests.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM

cc=${CC:-cc}
cxx=${CXX:-c++}

$cc -std=c11 -Wall -Wextra -Werror -I"$repo_root/main" \
    "$repo_root/tests/test_head_flow.c" \
    "$repo_root/main/head.c" \
    "$repo_root/main/flow.c" \
    -lm -o "$build_dir/test_head_flow"

$cxx -std=c++20 -Wall -Wextra -Werror -I"$repo_root/main" \
    "$repo_root/tests/test_vision_detector.cpp" \
    "$repo_root/main/vision_detector.cpp" \
    -o "$build_dir/test_vision_detector"

$cxx -std=c++20 -Wall -Wextra -Werror -I"$repo_root/main" \
    "$repo_root/tests/test_vision_dsp.cpp" \
    "$repo_root/main/vision_dsp.cpp" \
    -o "$build_dir/test_vision_dsp"

"$build_dir/test_head_flow"
"$build_dir/test_vision_detector"
"$build_dir/test_vision_dsp"

printf '%s\n' "Host tests passed."

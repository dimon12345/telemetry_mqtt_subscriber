#!/usr/bin/env bash

set -ex

cd "$(dirname "$(readlink -f "$0")")"

mkdir -p build

MACOS_CMAKE_FLAGS="-DCMAKE_CXX_COMPILER=clang++-mp-18 -DCMAKE_PREFIX_PATH=/usr/local"

if [ "$(uname -s)" = "Darwin" ]; then
    cmake ${MACOS_CMAKE_FLAGS} -B build
else
    cmake -B build
fi

cmake --build build -v

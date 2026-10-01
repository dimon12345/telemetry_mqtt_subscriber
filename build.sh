#!/usr/bin/env bash

set -ex

mkdir -p build

MACOS_CMAKE_FLAGS="-DCMAKE_CXX_COMPILER=clang++-mp-18 -DCMAKE_PREFIX_PATH=/usr/local"

if [ "$(uname -s)" = "Darwin" ]; then
    cmake ${MACOS_CMAKE_FLAGS} -B build
else
    cmake -B build
fi

cmake --build build

if [ -n "$DISABLE_POSTGRESQL" ] && [ "DISABLE_POSTGRESQL" != "0" ]; then
    DISABLE_POSTGRESQL_FLAGS=--disable-postgresql
fi

build/mqtt_subscriber -v ${DISABLE_POSTGRESQL_FLAGS}

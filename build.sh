#!/usr/bin/env bash

OLD_CWD=$CWD
mkdir -p build
rm -rf build/*
cd build
cmake ..
make && ./mqtt_subscriber --config ../config.json
cd $OLD_CWD

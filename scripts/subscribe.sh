#!/usr/bin/env bash

set -ex

mosquitto_sub -L "mqtt://telemetry:telemetry@localhost:1883/house/#"

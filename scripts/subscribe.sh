#!/usr/bin/env bash

set -ex

mosquitto_sub -i subscribe_script -L "mqtt://telemetry:telemetry@localhost:1883//house/#"

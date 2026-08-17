#!/usr/bin/env bash

cd "$(dirname "$(readlink -f "$0")")"

MQTT_HOST="${MQTT_HOST:-localhost}"
MQTT_USER="${MQTT_USER:-mqtt_subscriber}"
MQTT_PASSWORD="${MQTT_PASSWORD:-password}"

mosquitto_sub -i subscribe_script -L "mqtt://$MQTT_USER:$MQTT_PASSWORD@$MQTT_HOST:1883/house/#"

#!/bin/bash
set -ex

echo "Deploy MQTT subscriber"
cd "$(dirname "$(readlink -f "$0")")"

DISABLE_BENCHMARK_TEST=1 DISABLE_START_SUBSCRIBER=1 ./build.sh

SERVICE_NAME="telemetry_mqtt.service"
SERVICE_ETC_FILENAME="/etc/systemd/system/$SERVICE_NAME"

sudo cp "systemd/$SERVICE_NAME" "$SERVICE_ETC_FILENAME"
sudo chmod 644 "$SERVICE_ETC_FILENAME"

sudo systemctl daemon-reload
sudo systemctl enable "$SERVICE_NAME"
sudo systemctl restart "$SERVICE_NAME"
sudo systemctl status "$SERVICE_NAME" --no-pager

echo "Deploy MQTT subscriber successful finished"

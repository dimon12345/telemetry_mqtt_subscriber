#!/usr/bin/env bash

cd "$(dirname "$(readlink -f "$0")")"

MQTT_HOST="${MQTT_HOST:-localhost}"
MQTT_PUB_USER="${MQTT_PUB_USER:-mqtt_sensor}"
MQTT_PUB_PASSWORD="${MQTT_PUB_PASSWORD:-password}"

while true; do
    CURRENT_TIME=$(date +"%H %M %S" | awk '{print $1 + $2/60 + $3/3600}')

    # 30degrees * sin(time - 10hours). +30C at 14:00 and -30 at 02:00
    EXTREME_T=30
    VALUE_T=$(awk -v t="$CURRENT_TIME" 'BEGIN {
        pi = 3.14159265;
        val = 30 * sin((t - 10) * pi / 12);
        printf "%.2f", val
    }')

    echo "send fake_sensor temperature $VALUE_T"
    mosquitto_pub -h $MQTT_HOST -p 1883 -u "$MQTT_PUB_USER" -P "$MQTT_PUB_PASSWORD" -t "house/office/fake_sensor/state" -m '{"temperature":'"$VALUE_T"'}'

    sleep 60
done

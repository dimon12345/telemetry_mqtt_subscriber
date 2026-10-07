#!/bin/bash

while true; do
    CURRENT_TIME=$(date +"%H %M %S" | awk '{print $1 + $2/60 + $3/3600}')

    # 30degrees * sin(time - 10hours). +30C at 14:00 and -30 at 02:00
    EXTREME_T=30
    VALUE_T=$(awk -v t="$CURRENT_TIME" 'BEGIN {
        pi = 3.14159265;
        val = 30 * sin((t - 10) * pi / 12);
        printf "%.2f", val
    }')

    echo "send fake_sensor value $VALUE_T"
    mosquitto_pub -h mqtt-host -p 1883 -u "lexx" -P "xev" -t "house/office/fake_sensor" -m "$VALUE_T"

    sleep 60
done

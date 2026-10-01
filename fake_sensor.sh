#!/bin/bash

while true; do
    CURRENT_TIME=$(date +"%H %M %S" | awk '{print $1 + $2/60 + $3/3600}')
    EXTREME_T=30
    VALUE_T=$(awk -v t="$CURRENT_TIME" 'BEGIN {
        pi = 3.14159265;
        val = 30 * sin((t - 10) * pi / 12);
        printf "%.2f", val
    }')
    echo "send T value $VALUE_T"
    mosquitto_pub -h mqtt-host -p 1883 -u "lexx" -P "xev" -t "test" -m "sensor dht22#3#T $VALUE_T"
    sleep 60
done

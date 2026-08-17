#!/bin/bash

cd "$(dirname "$(readlink -f "$0")")"

MOSQUITTO_PATH=../mosquitto
MOSQUITTO_NEW_PATH=../mosquitto_new

rm -rf $MOSQUITTO_NEW_PATH
mkdir $MOSQUITTO_NEW_PATH

cat <<EOF > $MOSQUITTO_NEW_PATH/acl
user mqtt_subscriber
topic read house/#

user mqtt_sensor
topic write house/#
EOF

docker run --rm -it -v "$PWD/$MOSQUITTO_NEW_PATH:/auth" --entrypoint sh eclipse-mosquitto:2 \
 -c 'echo mqtt_subscriber password && mosquitto_passwd -c /auth/passwords mqtt_subscriber && \
echo mqtt_sensor password && mosquitto_passwd /auth/passwords mqtt_sensor && \
chown mosquitto:mosquitto /auth/passwords && chmod 600 /auth/passwords && \
chown mosquitto:mosquitto /auth/acl && chmod 600 /auth/acl'

if [ $? -ne 0 ]; then
    echo "docker returns error"
    exit 1
fi;

rm -rf $MOSQUITTO_PATH
mv $MOSQUITTO_NEW_PATH $MOSQUITTO_PATH

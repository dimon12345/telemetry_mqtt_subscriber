#!/usr/bin/env bash

if [[ "$1" == "--down" ]]; then
    docker compose -p mqtt_subscriber -f docker-compose.yml down -v
else
    docker compose -p mqtt_subscriber -f docker-compose.yml up --build --exit-code-from mqtt_subscriber
fi

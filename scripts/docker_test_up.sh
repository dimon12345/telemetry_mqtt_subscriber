#!/usr/bin/env bash

if [[ "$1" == "--down" ]]; then
    docker compose -p mqtt_subscriber_bench -f docker-compose.test.yml down -v
else
    docker compose -p mqtt_subscriber_bench -f docker-compose.test.yml up --build --exit-code-from bench_runner
fi

#!/usr/bin/env bash

DOCKER_COMPOSE_CONFIG=docker-compose.test.yml
#FORCE_RECREATE=--force-recreate

if [[ "$1" == "--down" ]]; then
    docker compose -p mqtt_subscriber_bench -f $DOCKER_COMPOSE_CONFIG  down -v
else
    BUILDKIT_PROGRESS=plain docker compose -p mqtt_subscriber_bench -f $DOCKER_COMPOSE_CONFIG up $FORCE_RECREATE --build --exit-code-from bench_runner
fi

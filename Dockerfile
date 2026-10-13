FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpq-dev \
    libpqxx-dev \
    libpaho-mqtt-dev \
    libpaho-mqttpp-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /telemetry
COPY . .

RUN mkdir docker_build && cmake -B docker_build -DCMAKE_BUILD_TYPE=Release && cmake --build docker_build

CMD sh -c "./docker_build/mqtt_subscriber"

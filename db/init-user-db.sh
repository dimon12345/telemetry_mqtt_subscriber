#!/usr/bin/env bash

psql -v ON_ERROR_STOP=1 --username "postgres" --dbname "telemetry" <<-EOSQL
    CREATE USER "telemetry" WITH PASSWORD 'telemetry';
    GRANT ALL PRIVILEGES ON DATABASE "telemetry" TO "telemetry";
EOSQL

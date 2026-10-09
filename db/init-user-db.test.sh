#!/usr/bin/env bash

psql -v ON_ERROR_STOP=1 --username "postgres" --dbname "telemetry_test" <<-EOSQL
    CREATE USER "telemetry" WITH PASSWORD 'telemetry';
    GRANT ALL PRIVILEGES ON DATABASE "telemetry_test" TO "telemetry";
EOSQL

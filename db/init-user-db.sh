#!/bin/bash
set -eu

: "${POSTGRES_USER:?}"
: "${POSTGRES_DB:?}"
: "${TELEMETRY_DB_USER:?}"
: "${TELEMETRY_DB_PASSWORD:?}"

psql -v ON_ERROR_STOP=1 \
    --username "$POSTGRES_USER" \
    --dbname "$POSTGRES_DB" \
    --set=db_name="$POSTGRES_DB" \
    --set=db_user="$TELEMETRY_DB_USER" \
    --set=db_password="$TELEMETRY_DB_PASSWORD" \
    --file=/docker-entrypoint-initdb.d/init_db.sql.template

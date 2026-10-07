#!/usr/bin/env bash

set -ex

cd "$(dirname "$(readlink -f "$0")")"

sudo -u postgres psql -d telemetry -v app_user="$POSTGRES_USER" -f init_db.sql

CREATE TABLE IF NOT EXISTS sensors (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS telemetry (
    id BIGSERIAL PRIMARY KEY,
    sensor_id BIGINT NOT NULL,
    value REAL NOT NULL,
    timestamp TIMESTAMPTZ NOT NULL,

    CONSTRAINT fk_sensor
        FOREIGN KEY(sensor_id)
        REFERENCES sensors(id)
        ON DELETE CASCADE
);

GRANT SELECT, INSERT ON TABLE telemetry TO telemetry;
GRANT USAGE, SELECT ON SEQUENCE telemetry_id_seq TO telemetry;
GRANT SELECT, INSERT ON TABLE sensors TO telemetry;
GRANT USAGE, SELECT ON SEQUENCE sensors_id_seq TO telemetry;

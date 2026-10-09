CREATE TABLE IF NOT EXISTS sensors (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS measurements (
    id BIGSERIAL PRIMARY KEY,
    sensor_id BIGINT NOT NULL,
    value REAL NOT NULL,
    timestamp TIMESTAMPTZ NOT NULL,

    CONSTRAINT fk_sensor
        FOREIGN KEY(sensor_id)
        REFERENCES sensors(id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_measurements_timestamp ON measurements(timestamp);

GRANT SELECT, INSERT, TRUNCATE ON TABLE measurements TO telemetry;
GRANT USAGE, SELECT ON SEQUENCE measurements_id_seq TO telemetry;
GRANT SELECT, INSERT ON TABLE sensors TO telemetry;
GRANT USAGE, SELECT ON SEQUENCE sensors_id_seq TO telemetry;

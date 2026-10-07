CREATE TABLE IF NOT EXISTS sensors (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS measurements (
    id BIGSERIAL PRIMARY KEY,
    sensor_id BIGINT NOT NULL,
    value REAL NOT NULL,
    timestamp TIMESTAMPTZ NOT NULL,

    -- Внешний ключ, связывающий измерение с датчиком
    CONSTRAINT fk_sensor
        FOREIGN KEY(sensor_id)
        REFERENCES sensors(id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_measurements_timestamp ON measurements(timestamp);

GRANT INSERT ON TABLE measurements TO lexx;
GRANT SELECT, INSERT ON TABLE sensors TO lexx;

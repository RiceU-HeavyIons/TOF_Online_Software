DROP TABLE IF EXISTS tdc_configs;
CREATE TABLE tdc_configs (
    id          INTEGER         PRIMARY KEY AUTOINCREMENT,
    name        VACHAR(256)     NOT NULL,
    length      INTEGER         NOT NULL,
    checksum    INTEGER         NOT NULL,
    hex_string  VARCHAR(256)    NOT NULL,
    bit_string  VARCHAR(1024)   NOT NULL
);
SELECT * FROM tdc_configs;
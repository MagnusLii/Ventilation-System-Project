CREATE DATABASE IF NOT EXISTS ventprojectdb;

USE ventprojectdb;

CREATE TABLE readings (
  `index` BIGINT PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE,
  speed INT,
  setpoint INT,
  auto BOOLEAN,
  pressure INT,
  co2 INT,
  ah INT,
  rh INT,
  temp INT,
  timestamp DATETIME
);

CREATE TABLE logmessages (
  `index` BIGINT PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE,
  message TEXT,
  timestamp DATETIME
);

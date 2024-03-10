CREATE DATABASE IF NOT EXISTS ventprojectdb;

USE ventprojectdb;

CREATE TABLE readings (
  `index` BIGINT PRIMARY KEY NOT NULL UNIQUE AUTO_INCREMENT,
  speed INT,
  pressure INT,
  co2 INT,
  ah INT,
  rh INT,
  temp INT,
  `timestamp` DATETIME
);

CREATE TABLE logmessages (
  `index` BIGINT PRIMARY KEY NOT NULL UNIQUE AUTO_INCREMENT,
  logcode TEXT,
  `timestamp` DATETIME
);

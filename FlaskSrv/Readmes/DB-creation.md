SQL query to create the DB for MariaDB.<br>
```SQL
CREATE DATABASE IF NOT EXISTS ventprojectdb;

USE ventprojectdb;

-- Create 'readings' table
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

-- Create 'logmessages' table
CREATE TABLE logmessages (
  `index` BIGINT PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE,
  message TEXT,
  timestamp DATETIME
);
```
```sql
-- Inserting data into the 'readings' table
INSERT INTO readings (speed, setpoint, auto, pressure, co2, ah, rh, temp, timestamp)
VALUES
  (120, 100, true, 30, 400, 60, 70, 25, '2024-03-11 08:30:00'),
  (90, 80, false, 25, 350, 50, 65, 23, '2024-03-11 09:00:00'),
  (150, 120, true, 35, 450, 70, 75, 28, '2024-03-11 09:30:00');

-- Inserting data into the 'logmessages' table
INSERT INTO logmessages (message, timestamp)
VALUES
  ('System startup', '2024-03-11 08:00:00'),
  ('Pressure alarm triggered', '2024-03-11 09:15:00'),
  ('Temperature out of range', '2024-03-11 10:00:00');
```
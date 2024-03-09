# Topics

- ##  **Ventilation controller messages topics**
    -   **Topic:** `controller/settings`
    -   **Description:** Keijos controller (and the website) send commands here to inform the Pi on how to proceed.
    -   **Example Payload:** 
```json
// Manual mode:
{
"auto": false,
"speed": 18
}
// Automatic mode:
{
"auto": true,
"pressure": 25
}
```
- ##  **Ventilation system readings topics**
    -   **Topic:** `controller/status`
    -   **Description:** The Pi (or maybe arduino??? Needs to be verified) sends readings here for something to read them??? IDK this is unclear and needs verification. The Flask server will read status updates from here.
    -   **Example Payload:** 
```json
{
"nr": 96,
"speed": 18,
"setpoint": 18,
"pressure": 5,
"auto": false,
"error": false,
"co2": 300,
"rh": 37,
"temp": 20
}
```
- ##  **Pi Loghandler log messages**
    -   **Topic:** `loghandler/logMessages`
    -   **Description:** The Pi sends log messages to this topic, the status is repsresnted by a predefined integer.
    -   **Example Payload:** 
```json
{
"log": "text"
}
```
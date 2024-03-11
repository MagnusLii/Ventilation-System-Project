# Topics

- ##  **Ventilation controller messages topics**
    -   **Topic:** `vent/controller/settings`
    -   **Description:** Keijos controller (and the website) send commands here to inform the Pi on how to proceed.
    -   **Example Payload:** 
```json
// Manual mode:
{
"auto": false,
"value": 18
}
```
- ##  **Ventilation system readings topics**
    -   **Topic:** `vent/controller/status`
    -   **Description:** The Pi (or maybe arduino??? Needs to be verified) sends readings here for something to read them??? IDK this is unclear and needs verification. The Flask server will read status updates from here.
    -   **Example Payload:** 
```json
{
"speed": 18,
"setpoint": 18,
"pressure": 5,
"auto": false,
"error": false,
"co2": 300,
"ah": 200,
"rh": 37,
"temp": 20 
}
```
- ##  **Pi Loghandler log messages**
    -   **Topic:** `loghandler/logMessages`
    -   **Description:** The Pi sends log messages to this topic.
    -   **Example Payload:** 
```json
{
"log": "text"
}
```
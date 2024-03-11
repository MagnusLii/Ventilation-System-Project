# API Endpoints Description

## 1. Get All Readings

### Endpoint
- **URL:** `/api/v0.1/readings/all`
- **Method:** `GET`

### Description
Retrieve all readings from the database.

### Usage
- Send a GET request to the specified endpoint.

---

## 2. Get Readings Since Timestamp

### Endpoint
- **URL:** `/api/v0.1/readings/since`
- **Method:** `POST`

### Description
Retrieve readings recorded since a specified timestamp.

### Request Body (JSON Format)
```json
{
    "timestamp": "2021-09-14 12:00:00"
}
```

### Usage
- Send a POST request to the specified endpoint with the timestamp in the request body.

---

## 3. Get All Logs

### Endpoint
- **URL:** `/api/v0.1/logs/all`
- **Method:** `GET`

### Description
Retrieve all logs from the database.

### Usage
- Send a GET request to the specified endpoint.

---

## 4. Get Logs Since Timestamp

### Endpoint
- **URL:** `/api/v0.1/logs/since`
- **Method:** `POST`

### Description
Retrieve logs recorded since a specified timestamp.

### Request Body (JSON Format)
```json
{
    "timestamp": "2021-09-14 12:00:00"
}
```

### Usage
- Send a POST request to the specified endpoint with the timestamp in the request body.

---

**Note:**
- Ensure that the timestamp is provided in the expected format ("YYYY-MM-DD HH:mm:ss").
- For the endpoints that expect a timestamp, an error response with status code 400 will be returned if the provided JSON format is invalid.
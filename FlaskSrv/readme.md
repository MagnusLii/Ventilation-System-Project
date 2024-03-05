# Ventilation System Project flask server readme
## Requirements
- Python 3.9.13+
- an SQL db
- MQTT broker
## Python Imports
### flask_mqtt
```bash
pip  install  Flask-MQTT
```
### sqlalchemy
```bash
pip  install  SQLAlchemy
```
### flask
```bash
pip  install  Flask
```
### flask CORS
```bash
pip  install  Flask-Cors
```
## Database
TODO:
### setup
TODO:
## Getting Started
The server requires python 3.9.13 or above, the libraries mentioned above, and the DB to be setup.<br>
#### DB setup
Once they're setup a file named `credentials.py` needs to be created under `ServerImports`. This file will contain the credentials needed to access your db. The full path being `<root>/ServerImports/credentials.py`.<br>
The file needs to contain the following variables:
```py
dbUsername = ''  # Username used to log into the DB
dbPassword = ''  # Password used to log into the DB
dbHostname = ''  # IP address of the machine running the DB, 'localhost' if running locally
dbPort = ''  # Port behind which the DB can be accessed
dbName = ''  # Name given to the DB on creation
```

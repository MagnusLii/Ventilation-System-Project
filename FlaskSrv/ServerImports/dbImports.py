from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import jsonify
from ServerImports import logHandler

db = SQLAlchemy()

class Readings(db.Model):
    index = db.Column(db.BIGINT, primary_key=True, nullable=False, unique=True, autoincrement=True)
    speed = db.Column(db.Integer)
    pressure = db.Column(db.Integer)
    co2 = db.Column(db.Integer)
    ah = db.Column(db.Integer)
    rh = db.Column(db.Integer)
    temp = db.Column(db.Integer)
    timestamp = db.Column(db.DateTime)

class LogMessages(db.Model):
    index = db.Column(db.BIGINT, primary_key=True, nullable=False, unique=True, autoincrement=True)
    logcode = db.Column(db.Text)
    timestamp = db.Column(db.DateTime)


def get_all_readings(app):
    """
    Get all readings from the database.

    Args:
        - app: The Flask app object.

    Returns:
        - JSON: A JSON object containing all readings from the database.
            Each reading is represented as a dictionary with the following keys.
            - index: The index of the reading in the database.
            - speed: The speed reading.
            - pressure: The pressure reading.
            - co2: The CO2 reading.
            - ah: The absolute humidity reading.
            - rh: The relative humidity reading.
            - temp: The temperature reading.
            - timestamp: The timestamp of the reading.
    
    Raises:
        - 500: If an error occurs while retrieving readings from the database.
    """
    try:
        with app.app_context():
            readings = Readings.query.all()
            json = jsonify([{
                'index': reading.index,
                'speed': reading.speed,
                'pressure': reading.pressure,
                'co2': reading.co2,
                'ah': reading.ah,
                'rh': reading.rh,
                'temp': reading.temp,
                'timestamp': reading.timestamp
            } for reading in readings])
            return json, 200
    except Exception as errorMsg:
        logHandler.log_error(f'get_all_readings(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve readings from the database.'}), 500


def get_readings_since_timestamp(app, timestamp):
    """
    Get all readings from the database that were recorded after a given timestamp.
    
    Args:
        - app: The Flask app object.
        - timestamp: The timestamp to compare against.
    
    Returns:
        - JSON: A JSON object containing all readings from the database that were recorded after the given timestamp.
            Each reading is represented as a dictionary with the following keys.
            - index: The index of the reading in the database.
            - speed: The speed reading.
            - pressure: The pressure reading.
            - co2: The CO2 reading.
            - ah: The absolute humidity reading.
            - rh: The relative humidity reading.
            - temp: The temperature reading.
            - timestamp: The timestamp of the reading.
    
    Raises:
        - 500: If an error occurs while retrieving readings from the database.
    """
    try:
        with app.app_context():
            readings = Readings.query.filter(Readings.timestamp >= timestamp).all()
            json = jsonify([{
                'index': reading.index,
                'speed': reading.speed,
                'pressure': reading.pressure,
                'co2': reading.co2,
                'ah': reading.ah,
                'rh': reading.rh,
                'temp': reading.temp,
                'timestamp': reading.timestamp
            } for reading in readings])
            return json, 200
    except Exception as errorMsg:
        logHandler.log_error(f'get_reading_since_timestamp(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve readings from the database.'}), 500


def push_reading(app, json_data):
    """
    Push a reading into the database with the current timestamp.

    Args:
        - app: The Flask app object.
        - json_data: A JSON object containing the reading data.
            The JSON object should have the following keys.
            - speed: The speed reading.
            - pressure: The pressure reading.
            - co2: The CO2 reading.
            - ah: The absolute humidity reading.
            - rh: The relative humidity reading.
            - temp: The temperature reading.

    Returns:
        - nothing: If the reading was successfully pushed into the database.

    Raises:
        - 500: If an error occurs while pushing the reading into the database.
    """
    try:
        with app.app_context():
            reading = Readings(
                speed=json_data['speed'],
                pressure=json_data['pressure'],
                co2=json_data['co2'],
                ah=json_data['ah'],
                rh=json_data['rh'],
                temp=json_data['temp'],
                timestamp=datetime.now()
            )
            db.session.add(reading)
            db.session.commit()
            return
    except Exception as errorMsg:
        logHandler.log_error(f'push_reading(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to push the reading into the database.'}), 500


def push_log(app, json_data):
    """
    Push a log message into the database with the current timestamp.

    Args:
        - app: The Flask app object.
        - json_data: A JSON object containing the log message data.
            The JSON object should have the following keys.
            - logcode: Containing the log message.
    
    Returns:
        - nothing: If the log message was successfully pushed into the database.

    Raises:
        - 500: If an error occurs while pushing the log message into the database.
    """
    try:
        with app.app_context():
            log = LogMessages(
                logcode=json_data['logcode'],
                timestamp=datetime.now()
            )
            db.session.add(log)
            db.session.commit()
            return
    except Exception as errorMsg:
        logHandler.log_error(f'push_log(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to push the log message into the database.'}), 500


def get_logs_since_timestamp(app, timestamp):
    """
    Get all logs from the database that were recorded after a given timestamp.
    
    Args:
        - app: The Flask app object.
        - timestamp: The timestamp to compare against.
    
    Returns:
        - JSON: A JSON object containing all logs from the database that were recorded after the given timestamp.
            Each log is represented as a dictionary with the following keys.
            - index: The index of the log in the database.
            - logcode: The log message.
            - timestamp: The timestamp of the log.
    
    Raises:
        - 500: If an error occurs while retrieving logs from the database.
    """
    try:
        with app.app_context():
            logs = LogMessages.query.filter(LogMessages.timestamp >= timestamp).all()
            json = jsonify([{
                'index': log.index,
                'logcode': log.logcode,
                'timestamp': log.timestamp
            } for log in logs])
            return json, 200
    except Exception as errorMsg:
        logHandler.log_error(f'get_logs_since_timestamp(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve logs from the database.'}), 500


def get_all_logs(app):
    """
    Get all logs from the database.

    Args:
        - app: The Flask app object.

    Returns:
        - JSON: A JSON object containing all logs from the database.
            Each log is represented as a dictionary with the following keys.
            - index: The index of the log in the database.
            - logcode: The log message.
            - timestamp: The timestamp of the log.
    
    Raises:
        - 500: If an error occurs while retrieving logs from the database.
    """
    try:
        with app.app_context():
            logs = LogMessages.query.all()
            json = jsonify([{
                'index': log.index,
                'logcode': log.logcode,
                'timestamp': log.timestamp
            } for log in logs])
            return json, 200
    except Exception as errorMsg:
        logHandler.log_error(f'get_all_logs(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve logs from the database.'}), 500
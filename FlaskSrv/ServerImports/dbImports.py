from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import jsonify
from ServerImports import logHandler
from ServerImports import ventrilator

db = SQLAlchemy()

class Readings(db.Model):
    __tablename__ = 'readings'
    index = db.Column(db.BIGINT, primary_key=True, autoincrement=True, unique=True, nullable=False)
    speed = db.Column(db.INT)
    setpoint = db.Column(db.INT)
    auto = db.Column(db.Boolean)
    pressure = db.Column(db.INT)
    co2 = db.Column(db.INT)
    ah = db.Column(db.INT)
    rh = db.Column(db.INT)
    temp = db.Column(db.INT)
    timestamp = db.Column(db.DATETIME)

class LogMessage(db.Model):
    __tablename__ = 'logmessages'
    index = db.Column(db.BIGINT, primary_key=True, autoincrement=True, unique=True, nullable=False)
    message = db.Column(db.TEXT)
    timestamp = db.Column(db.DATETIME)



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
            readings = Readings.query.order_by(Readings.index.desc()).all()
            json = jsonify([{
                'index': reading.index,
                'speed': reading.speed,
                'auto': reading.auto,
                'setpoint': reading.setpoint,
                'pressure': reading.pressure,
                'co2': reading.co2,
                'ah': reading.ah,
                'rh': reading.rh,
                'temp': reading.temp,
                'timestamp': reading.timestamp
            } for reading in readings])
            return json, 200
    except Exception as errorMsg:
        logHandler.log(f'get_all_readings(): {str(errorMsg)}')
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
            readings = Readings.query.filter(Readings.timestamp >= timestamp).order_by(Readings.index.desc()).all()
            json = jsonify([{
                'index': reading.index,
                'speed': reading.speed,
                'auto': reading.auto,
                'setpoint': reading.setpoint,
                'pressure': reading.pressure,
                'co2': reading.co2,
                'ah': reading.ah,
                'rh': reading.rh,
                'temp': reading.temp,
                'timestamp': reading.timestamp
            } for reading in readings])
            return json, 200
    except Exception as errorMsg:
        logHandler.log(f'get_reading_since_timestamp(): {str(errorMsg)}')
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
                auto=json_data['auto'],
                setpoint=json_data['setpoint'],
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
        logHandler.log(f'push_reading(): {str(errorMsg)}')
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
            log = LogMessage(
                logcode=json_data['logcode'],
                timestamp=datetime.now()
            )
            db.session.add(log)
            db.session.commit()
            return
    except Exception as errorMsg:
        logHandler.log(f'push_log(): {str(errorMsg)}')
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
            - message: The log message.
            - timestamp: The timestamp of the log.
    
    Raises:
        - 500: If an error occurs while retrieving logs from the database.
    """
    try:
        with app.app_context():
            logs = LogMessage.query.filter(LogMessage.timestamp >= timestamp).order_by(LogMessage.index.desc()).all()
            json = jsonify([{
                'index': log.index,
                'message': log.message,
                'timestamp': log.timestamp
            } for log in logs])
            return json, 200
    except Exception as errorMsg:
        logHandler.log(f'get_logs_since_timestamp(): {str(errorMsg)}')
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
            logs = LogMessage.query.order_by(LogMessage.index.desc()).all()
            json = jsonify([{
                'index': log.index,
                'message': log.message,
                'timestamp': log.timestamp
            } for log in logs])
            return json, 200
    except Exception as errorMsg:
        logHandler.log(f'get_all_logs(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve logs from the database.'}), 500


#TODO: finish this shit
def get_startup_values(app, ventrilator):
    """
    Get device mode and target value from last entry in the database.

    Args:
        - app: The Flask app object.
        - ventrilator: The ventrilator object.
        
    Returns:
        - Nothing
    """

    try:
        with app.app_context():
            readings = Readings.query.order_by(Readings.index.desc()).first()
            logHandler.log(f'get_startup_values(): setting vent obj to auto: {readings.auto}, valueTarget: {readings.setpoint}')
            ventrilator.setMode(readings.auto, readings.setpoint)
            logHandler.log(f'get_startup_values(): ventrilator obj auto: {ventrilator.auto}, valueTarget: {ventrilator.valueTarget}')
            return
    
    except Exception as errorMsg:
        logHandler.log(f'get_startup_values(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve startup values from the database.'}), 500
    

def get_latest_reading(app):
    """
    Get latest readings from DB.

    Args:
        - app: The Flask app object.

    Returns:
        - JSON: A JSON object containing latest readings from the database.
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
        - 500: If an error occurs while pushing the reading into the database.
    """

    try:
        with app.app_context():
            readings = Readings.query.order_by(Readings.index.desc()).first()
            json = jsonify({
                'index': readings.index,
                'speed': readings.speed,
                'auto': int(readings.auto),
                'setpoint': readings.setpoint,
                'pressure': readings.pressure,
                'co2': readings.co2,
                'ah': readings.ah,
                'rh': readings.rh,
                'temp': readings.temp,
                'timestamp': readings.timestamp
            })
            return json, 200
    except Exception as errorMsg:
        logHandler.log(f'get_all_readings(): {str(errorMsg)}')
        return jsonify({'error': 'Failed to retrieve readings from the database.'}), 500
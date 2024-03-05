from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import jsonify
from ServerImports import logHandler

db = SQLAlchemy()
# Database models will go 'ere once we have a layout.

class Readings(db.Model):
    index = db.Column(db.BIGINT, primary_key=True, nullable=False, unique=True, autoincrement=True)
    speed = db.Column(db.INT)
    pressure = db.Column(db.INT)
    co2 = db.Column(db.INT)
    ah = db.Column(db.INT)
    rh = db.Column(db.INT)
    temp = db.Column(db.INT)
    timestamp = db.Column(db.DATETIME)

def fetchAllBetween(app, start_datetime, end_datetime):
    try:
        with app.app_context():
            readings = Readings.query.filter(Readings.timestamp >= start_datetime, Readings.timestamp <= end_datetime).all()
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
        logHandler.log_error(f'error in fetchAllBetween(): {errorMsg}')
        return jsonify({'error': 'An error occurred while fetching readings.'}), 500


def fetchLatest(app):
    try:
        with app.app_context():
            latest = Readings.query.order_by(Readings.timestamp.desc()).first()
            json = jsonify({
                'index': latest.index,
                'speed': latest.speed,
                'pressure': latest.pressure,
                'co2': latest.co2,
                'ah': latest.ah,
                'rh': latest.rh,
                'temp': latest.temp,
                'timestamp': latest.timestamp
            })
            return json, 200
    except Exception as errorMsg:
        logHandler.log_error(f'error in fetchLatest(): {errorMsg}')
        return jsonify({'error': 'An error occurred while fetching latest reading.'}), 500
    

def insertReading(app, speed, pressure, co2, ah, rh, temp):
    try:
        with app.app_context():
            newReading = Readings(
                speed=speed,
                pressure=pressure,
                co2=co2,
                ah=ah,
                rh=rh,
                temp=temp,
                timestamp=datetime.now()
            )
            db.session.add(newReading)
            db.session.commit()
            return jsonify({'success': 'Reading added successfully.'}), 201
    except Exception as errorMsg:
        logHandler.log_error(f'error in insertReading(): {errorMsg}')
        return jsonify({'error': 'An error occurred while adding reading.'}), 500



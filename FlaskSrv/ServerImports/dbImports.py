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


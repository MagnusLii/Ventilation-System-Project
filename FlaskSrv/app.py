from flask import Flask, request, jsonify
from flask_mqtt import Mqtt
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from ServerImports import credentials
from ServerImports import mqttImports
from ServerImports import logHandler
from ServerImports import dbImports
from flask_cors import CORS
import threading
import time

# Flask app setup.
app = Flask(__name__)
CORS(app)

# MQTT setup.
app.config['MQTT_BROKER_URL'] = mqttImports.mqttBrokerIP
app.config['MQTT_BROKER_PORT'] = mqttImports.mqttBrokerPort
app.config['MQTT_KEEPALIVE'] = mqttImports.mqttKeepAliveSec
app.config['MQTT_TLS_ENABLED'] = False

# Database setup.
app.config['SQLALCHEMY_DATABASE_URI'] = f'mysql+pymysql://{credentials.dbUsername}:{credentials.dbPassword}@{credentials.dbHostname}:{credentials.dbPort}/{credentials.dbName}'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# Only for confirming that server is running.
@app.route('/')
def index():
    return 'Flask MQTT Server is running!'


# Subscribe to all topics in 'initialSubscribeTopics' list when server is started.
@mqttImports.mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
   if rc == 0:
       for topic in mqttImports.initialSubscribeTopics:
            mqttImports.mqtt.subscribe(topic, qos=1)  # subscribe to each topic
            logHandler.log(f'handle_message(), Subscribed to topic: {topic}')
   else:
        logHandler.log(f'handle_message(), Connection failed with result code {str(rc)}')


# MQTT message handling
@mqttImports.mqtt.on_message()
def handle_message(client, userdata, message):
    receivedMessage = message.payload.decode("utf-8")
    receivedTopic = message.topic
    logHandler.log(f'handle_message(), Received message: {receivedMessage} on topic: {receivedTopic}')

    # Decode JSON message.
    decodedMessage = mqttImports.decodeStringToJSON(receivedMessage)
    if decodedMessage == -1:
        logHandler.log(f'JSON decode failed.')
        return

    # Handle received message based on topic.
    if receivedTopic.startswith("controller/status") == True:
        """Expected JSON format:
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
        """
        logHandler.log(f'handle_message(): controller/status path.')
        logHandler.log(f'handle_message(): verifying keywords in JSON.')
        if mqttImports.validateKeywordsInJSON(decodedMessage, ['nr', 'speed', 'setpoint', 'pressure', 'auto', 'error', 'co2', 'rh', 'temp'], 2) == False:
            return
        
        logHandler.log(f'handle_message(): Publishing to database. -> dbImports.push_reading()')
        dbImports.push_reading(app, decodedMessage)
        return
    
    elif receivedTopic.startswith("loghandler/logMessages") == True:
        """Expected JSON format:
        {
        "log": "text"
        }
        """
        logHandler.log(f'handle_message(): loghandler/logMessages path.')
        logHandler.log(f'handle_message(): verifying keywords in JSON.')
        if mqttImports.validateKeywordsInJSON(decodedMessage, ['log'], 2) == False:
            return
        
        logHandler.log(f'handle_message(): Publishing to database. -> dbImports.push_log()')
        dbImports.push_log(app, decodedMessage)
        return
    
    logHandler.log(f'handle_message(): No path found for topic: {receivedTopic}')
    return # End of function.


# API endpoints
@app.route('/api/v0.1/readings/all', methods=['GET'])
def get_all_readings():
    logHandler.log(f'get_all_readings(): "/api/v0.1/readings/all" Request received.')
    return dbImports.get_all_readings(app)

@app.route('/api/v0.1/readings/since', methods=['POST'])
def get_reading_since_timestamp():
    """Expected JSON format:
    {
        "timestamp": "2021-09-14 12:00:00"
    }
    """
    if request.is_json:
        payload = request.json
        value = payload['timestamp']
        logHandler.log(f'get_reading_since_timestamp(): "/api/v0.1/readings/since" Request received. Timestamp: {value}')
        return dbImports.get_readings_since_timestamp(app, value)

    else:
        logHandler.log(f'get_reading_since_timestamp(): "/api/v0.1/readings/since" Request received. Invalid rquest format.')
        return jsonify({"error": "Invalid JSON format."}), 400


@app.route('/api/v0.1/logs/all', methods=['GET'])
def get_all_logs():
    logHandler.log(f'get_all_logs(): "/api/v0.1/logs/all" Request received.')
    return dbImports.get_all_logs(app)


@app.route('/api/v0.1/logs/since', methods=['POST'])
def get_logs_since_timestamp():
    """Expected JSON format:
    {
        "timestamp": "2021-09-14 12:00:00"
    }
    """
    if request.is_json:
        payload = request.json
        value = payload['timestamp']
        logHandler.log(f'get_logs_since_timestamp(): "/api/v0.1/logs/since" Request received. Timestamp: {value}')
        return dbImports.get_logs_since_timestamp(app, value)

    else:
        logHandler.log(f'get_logs_since_timestamp(): "/api/v0.1/logs/since" Request received. Invalid rquest format.')
        return jsonify({"error": "Invalid JSON format."}), 400


#def startup_procedures():
#    time.sleep(1) # Wait for app to be fully initialized, this is threaded so it won't block the main thread.
#    with app.app_context():
#        # Anything needed to be done at startup goes here.
#        return


if __name__ == '__main__':
    # Initialize imported app extensions.
    dbImports.db.init_app(app)
    mqttImports.mqtt.init_app(app)

    # Create a thread for startup procedures.
    startup_thread = threading.Thread(target=startup_procedures)

    # Start the thread.
    startup_thread.start()

    # Run the Flask application.
    app.run(host='0.0.0.0', port=5000, use_reloader=False)

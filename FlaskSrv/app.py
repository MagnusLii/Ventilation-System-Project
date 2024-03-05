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
    if receivedTopic.startswith("controller/settings") == True:
        # TODO: do something...
        return

    elif receivedTopic.startswith("controller/status") == True:
        # TODO: do something...
        return
    
    return # End of function.

def startup_procedures():
    time.sleep(3) # Wait for app to be fully initialized, this is threaded so it won't block the main thread.
    with app.app_context():
        # Anything needed to be done at startup goes here.
        return

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

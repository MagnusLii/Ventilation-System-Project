from flask_mqtt import Mqtt
import json
from ServerImports import logHandler

mqttBrokerPort = 1883 # 1883 is default
mqttKeepAliveSec = 10
mqttBrokerIP = '192.168.1.10'  # Replace with broker IP if not running locally.
mqttQoSLevel = 1 # TODO: verify what level the rest of the system uses and update if needed

mqtt = Mqtt()

# Topics
statusTopic = "vent/controller/status"
data = "data"

initialSubscribeTopics = [statusTopic, data] 

def decodeStringToJSON(json_string):
    """
    Decodes a JSON string to a Python dictionary.
    
    Args:
        - json_string (str): The JSON string to be decoded.
    
    Returns:
        - decodedMessage (dict): The decoded JSON string as a Python dictionary.
        - -1 (int): If the JSON string could not be decoded.
    """
    try:
        decodedMessage = json.loads(json_string)
        return decodedMessage
    except json.decoder.JSONDecodeError as error:
        logHandler.log(f'decodeStringToJSON(): JSON decode error: {error}')
        return -1


def validateKeywordsInJSON(decodedJSON, keywordList, verifycationLevel):
    """
    Validates the presence of a list of keywords in a JSON object.

    Args:
        - decodedJSON (dict): The decoded JSON object to be validated.
        - keywordList (list): A list of keywords to be validated.
        - verifycationLevel (int): The level of validation to be performed.
            - 1: Validates the presence of the keywords in the JSON object.
            - 2: Validates the presence of the keywords in the JSON object and that the values are not NULL or empty.
    
    Returns:
        - True (bool): If the JSON object passes the validation.
        - False (bool): If the JSON object fails the validation.
    
    Raises:
        - ValueError: If the verification level is not 1 or 2.
    """
    jsonKeySet = set(decodedJSON.keys())

    if verifycationLevel == 1:
        for keyword in keywordList:
            if keyword not in jsonKeySet:
                return False
        return True

    elif verifycationLevel == 2:
        for keyword in keywordList:
            if keyword not in jsonKeySet or not decodedJSON[keyword] or decodedJSON[keyword] == "NULL":
                return False
        return True

    else:
        logHandler.log(f'validateKeywordsInJSON(): Invalid verification level: {verifycationLevel}')
        raise ValueError("Invalid verification level. Please provide either 1 or 2.")


def publishJSONtoMQTT(topic, message):
    """
    Publishes a JSON message to a specified MQTT topic.

    Args:
        - topic (str): The MQTT topic to which the message will be published.
        - message (str): The JSON message to be published.
    
    Returns:
        - True (bool): If the message was successfully published.
        - False (bool): If the message failed to be published.
    """
    try:
        mqtt.publish(topic, message, qos=mqttQoSLevel)
    except:
        
        logHandler.log(f'publishJSONtoMQTT: Failed to publish message: {message} to topic: {topic}')
        return False
    else:
        return True

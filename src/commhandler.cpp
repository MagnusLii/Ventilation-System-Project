#include "commhandler.h"

#include "debugprint.h"

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

// Initializes the CommHandler with IPStack and MQTT client, sets MQTT version and client ID, and initializes predefined topics.
CommHandler::CommHandler(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client,
                         int mqtt_version, const char *client_id)
    : ipstack(ipstack), client(client) {
    // Set MQTT version and client ID in the connection data structure.
    connect_data.MQTTVersion = mqtt_version;
    connect_data.clientID.cstring = (char *)client_id;

    // Initialize predefined topics with their names and subscription status (0 means unsubscribed).
    topics = {{TopicType::DATA_TOPIC, std::make_pair("vent/controller/status", 0)},
              {TopicType::CONTROL_TOPIC, std::make_pair("vent/controller/settings", 0)},
              {TopicType::OTHER_TOPIC, std::make_pair("other", 0)},
              {TopicType::LOG_SEND, std::make_pair("loghandler/logMessages", 0)},
              {TopicType::STATUS_SEND, std::make_pair("loghandler/piStatusMessages", 0)}};
}

// Connects to a server using the provided hostname and port.
int CommHandler::connect_to_server(const char *hostname, int port) {
    DPRINT("Connecting to ", hostname, " on port ", port, "...");
    // Attempt to establish a TCP connection.
    int return_code = ipstack.connect(hostname, port);
    if (return_code != 0) {
        DPRINT("Failed TCP connect to ", hostname, ". RC: ", return_code);
    } else {
        DPRINT("Connected to ", hostname, ".");
    }

    return return_code;
}

// Connects to an MQTT broker using the previously set connection data.
int CommHandler::connect_to_broker() {
    DPRINT("MQTT connecting to broker...");
    int return_code = client.connect(connect_data);
    if (return_code != 0) {
        DPRINT("Failed to connect. RC: ", return_code);
    } else {
        DPRINT("MQTT connected to broker.");
    }

    return return_code;
}

// Subscribes to a topic using the previously set topic name.
int CommHandler::subscribe(TopicType topic_type) {
    const char *topic = topics.at(topic_type).first;
    return subscribe(topic_type, topic);
}

// Subscribes to a topic using the provided topic name.
int CommHandler::subscribe(TopicType topic_type, const char *topic) {
    int return_code = -2;

    // Check if already subscribed and unsubscribe if already subscribed to the same TopicType.
    if (is_subscribed(topic_type)) {
        DPRINT("Already subscribed to topic type ", topic_type, ".");
        if (unsubscribe(topic_type) != 0) {
            DPRINT("Failed to subscribe topic ", topic, ". RC: ", return_code);
            return return_code;
        }
    }

    // Update the topic if it's different from the current one.    
    if (topics.at(topic_type).first != topic) set_topic(topic_type, topic);

    DPRINT("MQTT subscribing to topic: ", topic, "...");
    return_code = client.subscribe(topic, MQTT::QOS2, message_arrived);
    if (return_code != 0) {
        DPRINT("Failed to subscribe to topic ", topic, ". RC: ", return_code);
    } else {
        DPRINT("MQTT subscribed to topic.");
        topics.at(topic_type).second = 1;
    }

    return return_code;
}

// Unsubscribes from a topic based on the given TopicType.
int CommHandler::unsubscribe(TopicType topic_type) {
    // Check if already unsubscribed and if so, return without performing further actions.
    if (topics.at(topic_type).second == 0) {
        DPRINT("Topic ", topics.at(topic_type).first, " already unsubscribed.");
        return 0;
    }

    // Get the topic name for the specified TopicType.
    const char *topic = topics.at(topic_type).first;

    DPRINT("MQTT unsubscribing from topic: ", topic, "...");
    int return_code = client.unsubscribe(topic);
    if (return_code != 0) {
        DPRINT("Failed to unsubscribe from topic ", topic, ". RC: ", return_code);
    } else {
        DPRINT("MQTT unsubscribed from topic.");
        topics.at(topic_type).second = 0;
    }

    return return_code;
}

// Publishes a message to a topic based on the given TopicType.
int CommHandler::publish(TopicType topic_type, const char *payload) {
    // Get the topic name for the specified TopicType.
    const char *topic = topics.at(topic_type).first;
    DPRINT("MQTT publishing: ", payload, " to topic ", topic, "...");
    // Create an MQTT message with the specified payload and QoS level 0.
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)payload;
    message.qos = MQTT::QOS0;
    message.payloadlen = strlen(payload) + 1;

    // Attempt to publish the message to the specified topic.
    int return_code = client.publish(topic, message);
    if (return_code != 0) {
        DPRINT("Failed to publish. RC: ", return_code);
    } else {
        DPRINT("MQTT message published.");
    }

    return return_code;
}

// Checks if the MQTT client is currently connected.
int CommHandler::verify_connection() { return client.isConnected(); }

// Reconnects to the MQTT broker if the client is not currently connected.
int CommHandler::reconnect() {
    DPRINT("MQTT not connected. Reconnecting...");
    int return_code = client.connect(connect_data);
    if (return_code != 0) {
        DPRINT("Failed to connect. RC: ", return_code);
    } else {
        DPRINT("MQTT reconnected.");
    }

    return return_code;
}

// Sets a new topic for a given TopicType.
void CommHandler::set_topic(TopicType topic_type, const char *new_topic) {
    topics.at(topic_type).first = new_topic;
}

// Checks if a topic is currently subscribed to.
int CommHandler::is_subscribed(TopicType topic_type) { return topics.at(topic_type).second; }

void CommHandler::send(int speed, int setpoint, int pressure, bool aut, bool error,
                  float co2, float ah, float rh, float temp) {
    

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();

    writer.Key("speed");
    writer.Int(speed);
    writer.Key("setpoint");
    writer.Int(get_set_point());
    writer.Key("pressure");
    writer.Int(pressure);
    writer.Key("auto");
    writer.Bool(!get_manual());
    writer.Key("error");
    writer.Bool(error);
    writer.Key("co2");
    writer.Double(co2);
    writer.Key("ah");
    writer.Double(ah);
    writer.Key("rh");
    writer.Double(rh);
    writer.Key("temp");
    writer.Double(temp);
    writer.EndObject();

    const char* jsonString = s.GetString();
    DPRINT(jsonString);
    publish(DATA_TOPIC, jsonString);
}


static volatile bool manual = false;
static volatile int setpoint = 50;
void message_arrived(MQTT::MessageData &data) {
    MQTT::Message &message = data.message;
    DPRINT("Message arrived: qos ", message.qos, ", retained: ", message.retained,
           ", dup: ", message.dup, ", packetid: ", message.id, "\n",
           "Payload: ", (char *)message.payload);

    rapidjson::Document doc;
    doc.Parse((char *)message.payload);
    const rapidjson::Value& aut = doc["auto"];
    const rapidjson::Value& set = doc["value"];

    setpoint = set.GetInt();
    manual = !(aut.GetInt());
}

bool get_manual() {
    return manual;
}
void set_manual(bool man) {
    manual = man;
}
int get_set_point() {
    return setpoint;
}

void set_set_point(int set_point) {
    setpoint = set_point;
}

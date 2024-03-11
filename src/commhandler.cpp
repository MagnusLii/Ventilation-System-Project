#include "commhandler.h"

#include "debugprint.h"

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

CommHandler::CommHandler(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client,
                         int mqtt_version, const char *client_id)
    : ipstack(ipstack), client(client) {
    connect_data.MQTTVersion = mqtt_version;
    connect_data.clientID.cstring = (char *)client_id;
    topics = {{TopicType::DATA_TOPIC, std::make_pair("vent/controller/status", 0)},
              {TopicType::CONTROL_TOPIC, std::make_pair("vent/controller/settings", 0)},
              {TopicType::OTHER_TOPIC, std::make_pair("other", 0)},
              {TopicType::LOG_SEND, std::make_pair("loghandler/logMessages", 0)},
              {TopicType::STATUS_SEND, std::make_pair("loghandler/piStatusMessages", 0)}};
}

int CommHandler::connect_to_server(const char *hostname, int port) {
    DPRINT("Connecting to ", hostname, " on port ", port, "...");
    int return_code = ipstack.connect(hostname, port);
    if (return_code != 0) {
        DPRINT("Failed TCP connect to ", hostname, ". RC: ", return_code);
        // goto cry; // or something more productive
        while (true)
            tight_loop_contents();
    } else {
        DPRINT("Connected to ", hostname, ".");
    }

    return return_code;
}

int CommHandler::connect_to_broker() {
    DPRINT("MQTT connecting to broker...");
    int return_code = client.connect(connect_data);
    if (return_code != 0) {
        DPRINT("Failed to connect. RC: ", return_code);
        // goto cry;
    } else {
        DPRINT("MQTT connected to broker.");
    }

    return return_code;
}

int CommHandler::subscribe(TopicType topic_type) {
    const char *topic = topics.at(topic_type).first;
    return subscribe(topic_type, topic);
}

int CommHandler::subscribe(TopicType topic_type, const char *topic) {
    int return_code = -2;
    if (is_subscribed(topic_type)) {
        DPRINT("Already subscribed to topic type ", topic_type, ".");
        if (unsubscribe(topic_type) != 0) {
            DPRINT("Failed to subscribe topic ", topic, ". RC: ", return_code);
            return return_code;
        }
    }

    if (topics.at(topic_type).first != topic) set_topic(topic_type, topic);

    DPRINT("MQTT subscribing to topic: ", topic, "...");
    return_code = client.subscribe(topic, MQTT::QOS2, message_arrived);
    if (return_code != 0) {
        DPRINT("Failed to subscribe to topic ", topic, ". RC: ", return_code);
        // goto cry;
    } else {
        DPRINT("MQTT subscribed to topic.");
        topics.at(topic_type).second = 1;
    }

    return return_code;
}

int CommHandler::unsubscribe(TopicType topic_type) {
    if (topics.at(topic_type).second == 0) {
        DPRINT("Topic ", topics.at(topic_type).first, " already unsubscribed.");
        return 0;
    }

    const char *topic = topics.at(topic_type).first;
    DPRINT("MQTT unsubscribing from topic: ", topic, "...");
    int return_code = client.unsubscribe(topic);
    if (return_code != 0) {
        DPRINT("Failed to unsubscribe from topic ", topic, ". RC: ", return_code);
        // goto cry;
    } else {
        DPRINT("MQTT unsubscribed from topic.");
        topics.at(topic_type).second = 0;
    }

    return return_code;
}

int CommHandler::publish(TopicType topic_type, const char *payload) {
    const char *topic = topics.at(topic_type).first;
    DPRINT("MQTT publishing: ", payload, " to topic ", topic, "...");
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)payload;
    message.qos = MQTT::QOS0;
    message.payloadlen = strlen(payload) + 1;

    int return_code = client.publish(topic, message);
    if (return_code != 0) {
        DPRINT("Failed to publish. RC: ", return_code);
        // goto cry;
    } else {
        DPRINT("MQTT message published.");
    }

    return return_code;
}

int CommHandler::verify_connection() { return client.isConnected(); }

int CommHandler::reconnect() {
    DPRINT("MQTT not connected. Reconnecting...");
    int return_code = client.connect(connect_data);
    if (return_code != 0) {
        DPRINT("Failed to connect. RC: ", return_code);
        // goto cry;
    } else {
        DPRINT("MQTT reconnected.");
    }

    return return_code;
}

void CommHandler::set_topic(TopicType topic_type, const char *new_topic) {
    topics.at(topic_type).first = new_topic;
}

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
int get_set_point() {
    return setpoint;
}

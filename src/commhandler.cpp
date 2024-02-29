#include "commhandler.h"

#include "debugprint.h"

CommHandler::CommHandler(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client,
                         int mqtt_version, const char *client_id, const char *topic)
    : ipstack(ipstack), client(client), topic(topic) {
    connect_data.MQTTVersion = mqtt_version;
    connect_data.clientID.cstring = (char *)client_id;
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
        while (true)
            tight_loop_contents();
    } else {
        DPRINT("MQTT connected to broker.");
    }

    return return_code;
}

int CommHandler::subscribe() {
    DPRINT("MQTT subscribing to topic: ", topic, "...");
    int return_code = client.subscribe(topic, MQTT::QOS2, message_arrived);
    if (return_code != 0) {
        DPRINT("Failed to subscribe to topic ", topic, ". RC: ", return_code);
        // goto cry;
        while (true)
            tight_loop_contents();
    }
    DPRINT("MQTT subscribed to topic.");

    return return_code;
}

int CommHandler::subscribe(const char *topic_name) {
    set_topic(topic_name);

    DPRINT("MQTT subscribing to topic: ", topic, "...");
    int return_code = client.subscribe(topic, MQTT::QOS2, message_arrived);
    if (return_code != 0) {
        DPRINT("Failed to subscribe to topic ", topic, ". RC: ", return_code);
        // goto cry;
        while (true)
            tight_loop_contents();
    }
    DPRINT("MQTT subscribed to topic.");

    return return_code;
}

int CommHandler::publish(const char *payload) {
    DPRINT("MQTT publishing: ", payload, "...");
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)payload;
    message.qos = MQTT::QOS0;
    message.payloadlen = strlen(payload) + 1;

    int return_code = client.publish(topic, message); // 0 = success, -1 = failure
    if (return_code != 0) {
        DPRINT("Failed to publish. RC: ", return_code);
        // goto cry;
        while (true)
            tight_loop_contents();
    }
    DPRINT("MQTT message published.");

    return return_code;
}

int CommHandler::verify_connection() { return client.isConnected(); }

int CommHandler::reconnect() {
    DPRINT("MQTT not connected. Reconnecting...");
    int return_code = client.connect(connect_data);
    if (return_code != 0) {
        DPRINT("Failed to connect. RC: ", return_code);
        // goto cry;
        while (true)
            tight_loop_contents();
    }
    DPRINT("MQTT reconnected.");

    return return_code;
}

void CommHandler::set_topic(const char *new_topic) { topic = new_topic; }

void message_arrived(MQTT::MessageData &data) {
    MQTT::Message &message = data.message;
    DPRINT("Message arrived: qos ", message.qos, ", retained: ", message.retained,
           ", dup: ", message.dup, ", packetid: ", message.id, "\n",
           "Payload: ", (char *)message.payload);
}

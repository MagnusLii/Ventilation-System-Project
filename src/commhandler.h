#pragma once

#include <map>

#include "pico/stdlib.h"

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"

typedef enum { DATA_TOPIC, CONTROL_TOPIC, OTHER_TOPIC, LOG_SEND, LOG_RECEIVE } TopicType;

class CommHandler {
  public:
    CommHandler(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client, int mqtt_version = 4, const char *client_id = "VSP");

    int connect_to_server(const char *hostname, int port);
    int connect_to_broker();
    int subscribe(TopicType topic_type);
    int subscribe(TopicType topic_type, const char *topic);
    int unsubscribe(TopicType topic_type);
    int publish(TopicType topic_type, const char *payload);
    int verify_connection();
    int reconnect();

  private:
    IPStack &ipstack;
    MQTT::Client<IPStack, Countdown> &client;
    MQTTPacket_connectData connect_data = MQTTPacket_connectData_initializer;
    // Something to store topics
    std::map<TopicType, std::pair<const char *, int>> topics;

    void set_topic(TopicType topic_type, const char *new_topic_name);
    int is_subscribed(TopicType topic_type);
};

void message_arrived(MQTT::MessageData &data);

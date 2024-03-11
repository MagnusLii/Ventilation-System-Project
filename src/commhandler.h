#pragma once

#include <map>

#include "pico/stdlib.h"

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"

typedef enum { DATA_TOPIC, CONTROL_TOPIC, OTHER_TOPIC, LOG_SEND, STATUS_SEND } TopicType;

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

    void send(int speed, int setpoint, int pressure, bool aut, bool error,
                  float co2, float ah, float rh, float temp);

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

bool get_manual();
int get_set_point();

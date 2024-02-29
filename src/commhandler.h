#include "pico/stdlib.h"

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"

class CommHandler {
  public:
    CommHandler(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client, int MQTTVersion = 4,
                const char *clientID = "VSP", const char *topic = "vsp-test");

    int connect_to_server(const char *hostname, int port);
    int connect_to_broker();
    int subscribe();
    int subscribe(const char *topic_name);
    int publish(const char *payload);
    int verify_connection();
    int reconnect();

  private:
    IPStack &ipstack;
    MQTT::Client<IPStack, Countdown> &client;
    MQTTPacket_connectData connect_data = MQTTPacket_connectData_initializer;
    const char *topic;

    void set_topic(const char *new_topic);
};

void message_arrived(MQTT::MessageData &data);

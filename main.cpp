#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "commhandler.h"

#define DEBUG_ENABLE
#include "debugprint.h"

int main() {
    stdio_init_all();

    DPRINT("Boot");
    const char *ssid = "SSID";
    const char *pw = "PW";
    const char *hostname = "0.0.0.0";
    int port = 1883;

    IPStack ipstack(ssid, pw);
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    CommHandler comm_handler(ipstack, client);

    if (comm_handler.connect_to_server(hostname, port)) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.connect_to_broker()) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.subscribe(TopicType::OTHER_TOPIC)) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.subscribe(TopicType::CONTROL_TOPIC)) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.subscribe(TopicType::DATA_TOPIC, "data")) {
        while (true) {
            tight_loop_contents();
        }
    }

    auto mqtt_send = make_timeout_time_ms(10000);
    int i = 0;

    while (true) {
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 10000);
            if (!comm_handler.verify_connection()) {
                if (!comm_handler.reconnect()) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
            }

            switch (i) {
            case 0:
                if (comm_handler.publish(TopicType::DATA_TOPIC, "Temp 2000 C")) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
                i++;
                break;
            case 1:
                if (comm_handler.publish(TopicType::CONTROL_TOPIC, "Fan to 100%")) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
                comm_handler.subscribe(TopicType::CONTROL_TOPIC, "control-2");
                i++;
                break;
            case 2:
                if (comm_handler.publish(TopicType::OTHER_TOPIC, "Never call this number again")) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
                comm_handler.unsubscribe(TopicType::OTHER_TOPIC);
                i = 0;
                break;
            default:
                if (comm_handler.publish(TopicType::OTHER_TOPIC, "Sumting wong")) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
                i = 0;
                break;
            }
        }

        // cyw43_arch_poll();
        client.yield(100);
    }

    return 0;
}

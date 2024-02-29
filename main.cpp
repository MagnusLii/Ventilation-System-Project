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
    IPStack ipstack("SSID", "PW");
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    CommHandler comm_handler(ipstack, client);
    
    if (comm_handler.connect_to_server("0.0.0.0", 1883)) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.connect_to_broker()) {
        while (true) {
            tight_loop_contents();
        }
    }

    if (comm_handler.subscribe()) {
        while (true) {
            tight_loop_contents();
        }
    }

    auto mqtt_send = make_timeout_time_ms(5000);

    while (true) {
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 5000);
            if (!comm_handler.verify_connection()) {
                if (!comm_handler.reconnect()) {
                    while (true) {
                        tight_loop_contents();
                    }
                }
            }

            const char* buf = "Test message";
            if (comm_handler.publish(buf)) {
                while (true) {
                    tight_loop_contents();
                }
            }
        }

        //cyw43_arch_poll();
        client.yield(100);
    }

    return 0;
}

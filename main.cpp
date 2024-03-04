#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"
#include "eeprom.h"
#include <iostream>
#include <string>
#include <vector>

#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "commhandler.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5

int main() {
    stdio_init_all();
    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);
int main() {
    stdio_init_all();

    DPRINT("Boot");
    const char *ssid = "ATHnet";
    const char *pw = "Joonas&Rosa";
    const char *hostname = "192.168.50.153";
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

    LogHandler logHandler;

    //logHandler.clearAllLogs(LOGTYPE_MSG_LOG); // clears all logs from addrs 0-2048
    //logHandler.clearAllLogs(LOGTYPE_REBOOT_STATUS); // clears all logs from addrs 2056-4096

    logHandler.pushLog(TEST); // pushes log to addr 0 (and 4096 ????)
    logHandler.pushLog(TEST2); // pushes log to addr 8 (and 2056 ????)
    logHandler.pushLog(TEST2); // pushes log to addr 16 (and 2064 ????)
    logHandler.pushLog(TEST2); // pushes log to addr 24 (and 2072 ????)
    logHandler.pushLog(TEST); // pushes log to addr 32 (and 2080 ????)

    logHandler.pushRebootLog(OK); // pushes log to addr 2056 (and 8 ????)
    logHandler.pushRebootLog(CRASH); // pushes log to addr 2064 (and 16 ????)
    logHandler.pushRebootLog(FORCED_REBOOT); // pushes log to addr 2072 (and 24 ????)
    logHandler.pushRebootLog(OK); // pushes log to addr 2080 (and 32 ????)

    printValidLogs(LOGTYPE_MSG_LOG); // prints logs from addrs 0-2048
    printValidLogs(LOGTYPE_REBOOT_STATUS); // prints logs from addrs 2048-4096
    return 0;
}

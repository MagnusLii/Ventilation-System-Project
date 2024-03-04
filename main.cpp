#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"
#include "eeprom.h"
#include <iostream>
#include <string>
#include <vector>
#include "hardware/timer.h"
#include "pico/time.h"
#include "commhandler.h"
#include <stdlib.h>

#define DEBUG_ENABLE
#include "debugprint.h"

#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5
    
int main() {
    stdio_init_all();
    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);
    DPRINT("Boot");
    const char *ssid = "CGA2121_BwQMKGs";
    const char *pw = "asd";
    const char *hostname = "192.168.0.15";
    int port = 1883;

    IPStack ipstack(ssid, pw);
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    CommHandler comm_handler(ipstack, client);

    comm_handler.connect_to_server(hostname, port);
    comm_handler.connect_to_broker();

    std::shared_ptr<CommHandler>commhandler_ptr = std::make_shared<CommHandler>(comm_handler);

    LogHandler loghandler;
    loghandler.setCommHandler(commhandler_ptr);
    loghandler.pushLog(TEST);

    return 0;
}

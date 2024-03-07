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


    LogHandler logHandler;

    for (int i = 0; i < 10; i++) {


    logHandler.printPrivates();

    std::string ssid = "ssid";
    std::string password = "password";
    std::string hostname = "127.0.0.1";
    std::string port = "1883";

    logHandler.storeCredentials(ssid, password, hostname, port);

    std::string ssid2;
    std::string password2;
    std::string hostname2;
    std::string port2;

    logHandler.fetchCredentials(&ssid2, &password2, &hostname2, &port2);

    }

    return 0;
}

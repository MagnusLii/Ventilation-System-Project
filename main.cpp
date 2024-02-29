#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"
#include "eeprom.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5

int main() {
    stdio_init_all();
    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);


    LogHandler logHandler(0, 0);
    uint8_t logArray[6] = {1, 3, 3, 7, 0, 0};
    int loglength = 6;
    
    std::cout << getChecksum(logArray, 6) << std::endl;

    enterLogToEeprom(logArray, &loglength, 0);

    uint8_t logArray2[20];
    eeprom_read_page(0, logArray2, 8);

    for (int i = 0; i < 8; i++) {
        std::cout << (int)logArray2[i] << " ";
    }
    std::cout << std::endl;

    int crc = getChecksum(logArray2, 6);
    std::cout << crc << std::endl;
    return 0;
}
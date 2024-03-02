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

    LogHandler logHandler;

    logHandler.zeroAllLogs(LOGTYPE_MSG_LOG);
    logHandler.zeroAllLogs(LOGTYPE_REBOOT_STATUS);
    //logHandler.pushLog(TEST2);
    //logHandler.pushRebootLog(OK);
    printValidLogs(LOGTYPE_MSG_LOG);
    printValidLogs(LOGTYPE_REBOOT_STATUS);
    return 0;
}
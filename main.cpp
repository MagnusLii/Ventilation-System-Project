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
    LogHandler logHandler;

    logHandler.zeroAllLogs(LOGTYPE_MSG_LOG);
    logHandler.zeroAllLogs(LOGTYPE_REBOOT_STATUS);

    logHandler.printPrivates();

    logHandler.pushLog(TEST2);

    logHandler.printPrivates();

    logHandler.pushRebootLog(OK);

    logHandler.printPrivates();

    printValidLogs();
    return 0;
}
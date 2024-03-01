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
    
    loghandler.zeroAllLogs(LOGTYPE_MSG_LOG);
    loghandler.zeroAllLogs(LOGTYPE_REBOOT_STATUS);

    loghandler.printPrivates();

    loghandler.pushLog(TEST2);

    loghandler.printPrivates();

    loghandler.pushRebootLog(OK);

    loghandler.printPrivates();

    printValidLogs();
    return 0;
}
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

    logHandler.clearAllLogs(LOGTYPE_MSG_LOG); // clears all logs from addrs 0-2048
    logHandler.clearAllLogs(LOGTYPE_REBOOT_STATUS); // clears all logs from addrs 2056-4096

    
    logHandler.pushLog(TEST); // pushes log to addr 0 (and 4096 ????)
    /*logHandler.pushLog(TEST2); // pushes log to addr 8 (and 2056 ????)
    logHandler.pushLog(TEST2); // pushes log to addr 16 (and 2064 ????)
    logHandler.pushLog(TEST2); // pushes log to addr 24 (and 2072 ????)
    logHandler.pushLog(TEST); // pushes log to addr 32 (and 2080 ????)
    */

    /*
    logHandler.pushRebootLog(OK); // pushes log to addr 2056 (and 8 ????)
    logHandler.pushRebootLog(CRASH); // pushes log to addr 2064 (and 16 ????)
    logHandler.pushRebootLog(FORCED_REBOOT); // pushes log to addr 2072 (and 24 ????)
    logHandler.pushRebootLog(OK); // pushes log to addr 2080 (and 32 ????)
    */

    printValidLogs(LOGTYPE_MSG_LOG); // prints logs from addrs 0-2048
    printValidLogs(LOGTYPE_REBOOT_STATUS); // prints logs from addrs 2048-4096
    return 0;
}
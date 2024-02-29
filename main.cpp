#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"

#define DEBUG_ENABLE
#include "debugprint.h"


int main() {
    stdio_init_all();
    int a = 42;
    DPRINT("AA", "bbb", a);

    LogHandlingWrapper::calculateCRC16("123456789", 9);

    return 0;
}
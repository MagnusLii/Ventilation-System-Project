#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#define DEBUG_ENABLE
#include "debugprint.h"
#include "pin.h"

int main() {
    stdio_init_all();
    int a = 42;
    DPRINT("AA", "bbb", a);

    GpioPin(10, false, false, true);
    GpioPin(11, false, false, true);

    while(true) {}
    return 0;
}
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
    RotaryEncoder A; //Create an object for rotary encoder.

    while(true) {
        std::cout << A.returnVal() << std::endl; //shows that the data is changed and can be accessed
        sleep_ms(5000);
    }
    return 0;
}
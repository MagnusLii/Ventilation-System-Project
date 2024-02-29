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


    return 0;
}
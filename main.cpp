#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#define DEBUG_ENABLE
#include "debugprint.h"
#include "pin.h"
#include "input.h"
#include "display/ssd1306.h"
#include "display/graphics.h"

int main() {
    stdio_init_all();
    int a = 42;
    DPRINT("AA", "bbb", a);
    RotaryEncoder A;
    mainMenuNon();
    while(true) {
        a = A.returnVal();
        if (a > 0 && a < 33) {
            mainMenuFirst();
        }else if(a > 33 && a < 66 ) {
            mainMenuSecond();
        } else if(a > 66 && a <= 100) {
            mainMenuThird();
        }

    }
    return 0;
}
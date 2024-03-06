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

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups

    int a = 42;
    ssd1306 display(i2c1);
    DPRINT("AA", "bbb", a);

    RotaryEncoder A;
    mainMenu(display);
    display.show();
    while(true) {
        a = A.returnVal();
        if (a > 0 && a < 33) {
            mainMenuFirst(display);
            display.show();
        }else if(a > 33 && a < 66 ) {
            mainMenuSecond(display);
            display.show();
        } else if(a > 66 && a <= 100) {
            mainMenuThird(display);
            display.show();
        }

    }
    return 0;
}
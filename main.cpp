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
#include "display/font_petme128_8x8.h"

int main() {
    stdio_init_all();

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups

    int a = 1;
    ssd1306 display(i2c1);
    DPRINT("AA", "bbb", a);

    Button button(7);
    Button button1(8);
    Button button2(9);
    RotaryEncoder A;
    while(true) {

        a = A.returnVal() % 6;
        int b = A.returnVal() + 12;
        mainMenu(display, button, a, b, b, 100);
        display.show();

        /*
        textInput(display, button, A.returnVal());
        display.show();
        if (button.returnState() && button.returnPin() == 12) {
            button.setState();
            DPRINT(returnInput());
        }
        */
    }
    return 0;
}
#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"

#include "uart_instance.h"
#include "i2c_instance.h"

#include "modbus_controller.h"
#include "modbus_register.h"
#include "pressure.h"
#include "fan.h"
#include "eeprom.h"
#include <iostream>
#include <string>
#include <vector>
#include "hardware/timer.h"
#include "pico/time.h"
#include "commhandler.h"
#include <stdlib.h>

#define DEBUG_ENABLE
#include "debugprint.h"
#include "pin.h"
#include "input.h"
#include "display/ssd1306.h"
#include "display/graphics.h"
#include "display/font_petme128_8x8.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define I2C1_SDA 14
#define I2C1_SCL 15
#define I2C1_BAUD 100000


bool user_input(char *dst, int size) {
    if (uart_is_readable(uart0)) {
        char c = 0;
        int i = 0;
        while (c != '\n' && i < size) {
            c = fgetc(stdin);
            dst[i] = c;
            i++;
        }
        dst[--i] = '\0';
        return true;
    }
    return false;
}
#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5
    
int main() {
    stdio_init_all();

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups

    int a = 1;
    ssd1306 display(i2c1);
    DPRINT("AA", "bbb", a);
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_i2c i2c{std::make_shared<I2C_instance>(i2c1, I2C1_BAUD, I2C1_SDA, I2C1_SCL)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};

    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister co(mbctrl, 240, 0, 2);

    WriteRegister fan_speed(mbctrl, 1, 0, 1);
    ReadRegister fan_counter(mbctrl, 1, 4, 1, false);
    PressureRegister pre(i2c, 64);
    FAN fan(&fan_speed, &fan_counter, &pre);

    char buf[20];
    int target = 50;
    bool spinning = false;
    while (1) {
        if (user_input(buf, 20)) {
            sscanf(buf, "%d", &target);
        }
        fan.adjust_speed(target);
        spinning = fan.is_spinning();
        DPRINT("pressure: ", fan.get_pressure(), " target: ", target, " fanspeed: ", fan.get_speed(), " spinning: ", spinning);
        sleep_ms(500);
    }
    
    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);

    LogHandler logHandler;

    for (int i = 0; i < 10; i++) {


    logHandler.printPrivates();

    std::string ssid = "ssid";
    std::string password = "password";
    std::string hostname = "127.0.0.1";
    std::string port = "1883";

    logHandler.storeCredentials(ssid, password, hostname, port);

    std::string ssid2;
    std::string password2;
    std::string hostname2;
    std::string port2;



    }

    Button button(7);
    Button button1(8);
    Button button2(9);
    RotaryEncoder A;
    while(true) {
        /*
        a = A.returnVal() % 6;
        startMenu(display, a);
        display.show();
        */
        textInput(display, button, A.returnVal());
        display.show();
        if (button.returnState() && button.returnPin() == 12) {
            button.setState();
            DPRINT(returnInput());
        }
    }
    return 0;
}

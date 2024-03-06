#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#include "uart_instance.h"
#include "i2c_instance.h"

#include "modbus_controller.h"
#include "modbus_register.h"
#include "pressure.h"
#include "fan.h"

#define DEBUG_ENABLE
#include "debugprint.h"


#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define I2C1_SDA 14
#define I2C1_SCL 15
#define I2C1_BAUD 100000


#include <cmath>
int adjust_fan(int pressure, int target) {
    if (abs(pressure - target) < 5) return 0;
    return (target - pressure) / 2;
}

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

int main() {
    stdio_init_all();

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
    

    
    return 0;
}
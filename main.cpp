#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "uart_instance.h"
#include "modbus_controller.h"
#include "modbus_register.h"
#include "pressure.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5


int main() {
    stdio_init_all();

    uint sda_pin = 14;
    uint scl_pin = 15;

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    gpio_set_dir(sda_pin, GPIO_OUT);
    gpio_set_dir(scl_pin, GPIO_OUT);

    i2c_init(i2c1, 100000);

    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};
    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister co(mbctrl, 240, 0, 2);
    WriteRegister mio(mbctrl, 1, 0, 1);
    ReadRegister dummy(mbctrl, 240, 12345);

    PressureRegister pre(1, 64);

    volatile int val = pre.get();
    DPRINT(val);

 

    uint16_t sped = 1000;
    mio.start_transfer(sped);
    //mio2.start_transfer(500);

    while(true) {
        val = pre.get();
        DPRINT(val);
        
        sleep_ms(1000);
    }

// while(1) tight_loop_contents();
    // co.start_transfer();
    // while (mbctrl->isbusy()) tight_loop_contents();
    // DPRINT(dummy.is_ok());
    // DPRINT(co.get_float());

    while(1) tight_loop_contents();

    return 0;
}
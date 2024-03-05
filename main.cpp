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
#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5




int main() {
    stdio_init_all();

    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);

    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};
    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister co(mbctrl, 240, 0, 2);
    WriteRegister mio(mbctrl, 1, 0, 1);
    ReadRegister dummy(mbctrl, 240, 12345);

    PressureRegister pre(0, 0x81);

    int val = pre.get();
    DPRINT(val);

 

    // uint16_t sped = 1000;
    // mio.start_transfer(sped);
    //mio2.start_transfer(500);

// while(1) tight_loop_contents();
    co.start_transfer();
    while (mbctrl->isbusy()) tight_loop_contents();
    DPRINT(dummy.is_ok());
    DPRINT(co.get_float());

    while(1) tight_loop_contents();

    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#include "uart_instance.h"
#include "modbus_register.h"
#include "modbus_controller.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5


int main() {
    stdio_init_all();
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};
    MODBUSRegister rh(mbctrl, 241, 256, true);
    MODBUSRegister mio1(mbctrl, 1, 0, false);
    MODBUSRegister mio2(mbctrl, 1, 1, false);


    mio1.start_transfer(1000);
    //mio2.start_transfer(500);

    rh.start_transfer();

    while(1) rh.start_transfer();

    return 0;
}
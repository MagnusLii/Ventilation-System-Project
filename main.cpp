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
    ReadRegister rh(mbctrl, 241, 256);

    rh.start_read();

    while(1) rh.start_read();

    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#include "uart_instance.h"
#include "modbus_controller.h"
#include "modbus_register.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5


int main() {
    stdio_init_all();
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};
    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister co(mbctrl, 240, 0, 2);
    WriteRegister mio(mbctrl, 1, 0, 1);

 


    // mio.start_transfer((uint16_t)129);
    //mio2.start_transfer(500);

// while(1) tight_loop_contents();
    co.start_transfer();
    while (mbctrl->isbusy()) tight_loop_contents();
    DPRINT(co.is_ok());

    while(1) tight_loop_contents();

    return 0;
}
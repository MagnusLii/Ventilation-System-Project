#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#include "uart_instance.h"
#include "modbus.h"

#define DEBUG_ENABLE
#include "debugprint.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5


int main() {
    stdio_init_all();
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    ReadRegister rh(u, 241, 256);

    rh.start_read();
    while(!rh.isready()) tight_loop_contents();
    while(1) tight_loop_contents();

    return 0;
}
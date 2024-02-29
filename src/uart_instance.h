#pragma once
#include "hardware/uart.h"

class Uart_instance {
    public:
        Uart_instance(uint uart_number, uint baudrate, uint data_bits uint parity, uint stop_bits);
    private:
        uart_inst_t *uart;
        uint baudrate;
};

Uart_instance::Uart_instance(uint uart_number, uint baudrate, uint data_bits, uint parity, uint stop_bits) :
baudrate(baudrate) {
    this->uart = (uart_number) ? uart1 : uart0;
    uart_init(this->uart, baudrate);
    uart_set_format(this->uart)
}
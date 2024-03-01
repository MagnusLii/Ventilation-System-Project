#pragma once

#include "pico/stdlib.h"
#include "hardware/uart.h"

class Uart_instance {
    public:
        Uart_instance(uint uart_number, uint baudrate, uint TX_pin, uint RX_pin);
        io_rw_32 *get_dr_address(void);
        uint get_index(void);
    private:
        uart_inst_t *uart;
        uint baudrate;
};

Uart_instance::Uart_instance(uint uart_number, uint baudrate, uint TX_pin, uint RX_pin) :
baudrate(baudrate) {
    gpio_set_function(TX_pin, GPIO_FUNC_UART);
    gpio_set_function(RX_pin, GPIO_FUNC_UART);
    this->uart = (uart_number) ? uart1 : uart0;
    uart_init(this->uart, baudrate);
}

io_rw_32 *Uart_instance::get_dr_address(void) {
    return &(uart_get_hw(this->uart)->dr);
}

uint Uart_instance::get_index(void) {
    return uart_get_index(uart);
}
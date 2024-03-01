
#include "uart_instance.h"

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
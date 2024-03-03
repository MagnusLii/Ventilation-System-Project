
#include "pico/time.h"
#include "modbus_controller.h"

ModbusCtrl::ModbusCtrl(shared_uart uart_pointer) :
    uart_ptr(uart_pointer), uart_baud(uart_pointer->get_baud()), busy(false), ctrl_time(0) {}


static inline uint64_t get_char_delay(uint baud) {
    // 1/baud = time it takes to send one bit
    // bit time * 8 = time it takes to send one character (8 bits)
    // byte time * 1000000 = byte time in microseconds
    // byte time us * 4 = time it takes to send 4 characters in microseconds
    return ((1/baud)*8*1000000*4);
}

void ModbusCtrl::start(void) {
    // if busy and not enough delay for next transfer wait
    while(busy && ((time_us_64() - ctrl_time) < get_char_delay(uart_baud))) tight_loop_contents();
    busy = true;
}

void ModbusCtrl::end(void) {
    ctrl_time = time_us_64();
    busy = false;
}
#include "pico/stdlib.h"

#include "modbus.h"

MODBUSRegister::MODBUSRegister(std::shared_ptr<Uart_instance> uart_pointer) :
txbuf(uart_pointer, nullptr), rxbuf(uart_pointer) {
    
}
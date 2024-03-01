#include "pico/stdlib.h"

#include "modbus.h"

#define FUNC_READ_HOLDING_REGISTER 0x3
#define FUNC_WRITE_MULTIPLE_REGISTERS 0x10

#define READ_EXPECTED_CHARACTERS 9

/*
This function is shamelessly copied from nanomodbus library
*/
static uint16_t crc(const uint8_t* data, uint32_t length) {
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < length; i++) {
        crc ^= (uint16_t) data[i];
        for (int j = 8; j != 0; j--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }
    return (uint16_t) (crc << 8) | (uint16_t) (crc >> 8);
}

MODBUSRegister::MODBUSRegister(shared_uart uart_pointer) :
txbuf(uart_pointer, nullptr), rxbuf(uart_pointer) {

}

bool MODBUSRegister::isready() {
    return rxbuf.isready();
}


ReadRegister::ReadRegister(shared_uart uart_pointer, uint8_t device_address, uint16_t register_address) :
MODBUSRegister(uart_pointer) {
     payload.devaddr = device_address;
     payload.function = FUNC_READ_HOLDING_REGISTER;
     payload.regaddr = register_address;
     payload.write_count = 1;
     payload.crc = crc((uint8_t *)&payload, sizeof(payload) - 2);

     txbuf.set_tx_buffer((uint8_t *)&payload);
}


void ReadRegister::start_read(void) {
    rxbuf.start_listening(READ_EXPECTED_CHARACTERS);
    txbuf.set_transfer_in_flight(sizeof(payload));
}
#pragma once

#include "pico/stdlib.h"
#include "modbus_controller.h"

#define PAYLOAD_MAX_LEN 32


class MODBUSRegister {
    public:
        MODBUSRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address, bool read);
        void start_transfer(uint16_t data=0);
    protected:
        shared_modbus modbus;
        uint8_t payload[PAYLOAD_MAX_LEN];
        uint8_t payload_len;
        uint8_t rxbuf[PAYLOAD_MAX_LEN];
        bool read;
};

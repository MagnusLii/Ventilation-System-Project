#pragma once

#include "pico/stdlib.h"
#include "dma_buffer.h"
#include "modbus_controller.h"

#define PAYLOAD_MAX_LEN 32


class MODBUSRegister {
    public:
        MODBUSRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address);
        virtual void done() = 0;
    protected:
        shared_modbus modbus;
        DMATXBuffer txbuf;
        DMARXBuffer rxbuf;
        uint8_t payload[PAYLOAD_MAX_LEN];
        uint8_t payload_len;
};

class ReadRegister : public MODBUSRegister {
    public:
        ReadRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address);
        void start_read(void);
        void done() override;
};
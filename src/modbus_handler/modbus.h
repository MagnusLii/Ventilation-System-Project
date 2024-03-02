#pragma once

#include "pico/stdlib.h"
#include "dma_buffer.h"

#define PAYLOAD_MAX_LEN 32


class MODBUSRegister {
    public:
        MODBUSRegister(shared_uart uart_pointer, uint8_t device_address, uint16_t register_address);
        bool isready();
    protected:
        DMATXBuffer txbuf;
        DMARXBuffer rxbuf;
        uint8_t payload[PAYLOAD_MAX_LEN];
        uint8_t payload_len;
};

class ReadRegister : public MODBUSRegister {
    public:
        ReadRegister(shared_uart uart_pointer, uint8_t device_address, uint16_t register_address);
        void start_read(void);
};
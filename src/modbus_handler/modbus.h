#pragma once

#include "pico/stdlib.h"
#include "dma_buffer.h"


struct ReadPayload {
    uint8_t devaddr;
    uint8_t function;
    uint16_t regaddr;
    uint16_t write_count;
    uint16_t crc;
} __packed;


class MODBUSRegister {
    public:
        MODBUSRegister(shared_uart uart_pointer);
        bool isready();
    protected:
        DMATXBuffer txbuf;
        DMARXBuffer rxbuf;
};


class ReadRegister : public MODBUSRegister {
    public:
        ReadRegister(shared_uart uart_pointer, uint8_t device_address, uint16_t register_address);
        void start_read(void);
    private:
        ReadPayload payload;
};


class WriteRegister {
    WriteRegister(shared_uart uart_pointer, uint8_t device_address, uint16_t register_address);
};


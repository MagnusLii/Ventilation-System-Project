#pragma once

#include "pico/stdlib.h"
#include "dma_buffer.h"


struct PayloadHeader {
    uint8_t devaddr;
    uint8_t function;
    uint16_t regaddr;
    uint16_t write_count;
} __packed;

class MODBUSRegister {
    public:
        MODBUSRegister(std::shared_ptr<Uart_instance> uart_pointer);

    protected:
        DMATXBuffer txbuf;
        DMARXBuffer rxbuf;
};


class WriteRegister {
    WriteRegister(uint8_t address);
};

class ReadRegister {
    ReadRegister(uint8_t address);
};

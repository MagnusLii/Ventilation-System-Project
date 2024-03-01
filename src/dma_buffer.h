#pragma once

#include <string>
#include <vector>
#include <memory>
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "uart_instance.h"
#define BUFFER_LENGTH 128

class DMABuffer {
    public:
        DMABuffer(std::shared_ptr<Uart_instance> uart_pointer);
        ~DMABuffer();
    protected:
        int dma_channel;
        std::shared_ptr<Uart_instance> uart;
};

class DMARXBuffer : DMABuffer {
    public:
        DMARXBuffer(std::shared_ptr<Uart_instance> uart_pointer);
    private:
        uint8_t buffer[BUFFER_LENGTH];
};

class DMATXBuffer : DMABuffer {
    public:
        DMATXBuffer(std::shared_ptr<Uart_instance> uart_pointer, uint8_t *buffer);
        void set_tx_buffer(uint8_t *buffer);
    private:
        uint8_t *buffer;
};
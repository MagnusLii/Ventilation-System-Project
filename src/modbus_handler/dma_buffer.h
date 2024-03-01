#pragma once

#include <string>
#include <vector>
#include <memory>
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "uart_instance.h"
#define BUFFER_LENGTH 128

class DMABuffer {
    public:
        DMABuffer(shared_uart uart_pointer);
        ~DMABuffer();
        void irq_hand();
        bool isready();
    protected:
        int dma_channel;
        shared_uart uart;
        bool ready;
};

class DMARXBuffer : public DMABuffer {
    public:
        DMARXBuffer(shared_uart uart_pointer);
        void start_listening(uint8_t max_characters);
    private:
        volatile uint8_t buffer[BUFFER_LENGTH];
};

class DMATXBuffer : public DMABuffer {
    public:
        DMATXBuffer(shared_uart uart_pointer, uint8_t *buffer);
        void set_tx_buffer(uint8_t *buffer);
        void set_transfer_in_flight(uint8_t transfer_count);
    private:
        uint8_t *buffer;
};
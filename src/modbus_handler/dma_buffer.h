#pragma once

#include <memory>
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "uart_instance.h"

class MODBUSRegister; // do this to avoid circular dependencies

#define BUFFER_LENGTH 128

class DMABuffer {
    public:
        DMABuffer(shared_uart uart_pointer, MODBUSRegister *owner);
        ~DMABuffer();
        void irq_hand();
    protected:
        int dma_channel;
        shared_uart uart;
        MODBUSRegister *owner;
};

class DMARXBuffer : public DMABuffer {
    public:
        DMARXBuffer(shared_uart uart_pointer, MODBUSRegister *owner);
        void start_listening(uint8_t max_characters);
    private:
        volatile uint8_t buffer[BUFFER_LENGTH];
};

class DMATXBuffer : public DMABuffer {
    public:
        DMATXBuffer(shared_uart uart_pointer, uint8_t *buffer, MODBUSRegister *owner);
        void set_tx_buffer(uint8_t *buffer);
        void set_transfer_in_flight(uint8_t transfer_count);
    private:
        uint8_t *buffer;
};
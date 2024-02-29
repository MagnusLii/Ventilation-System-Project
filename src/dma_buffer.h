#pragma once

#include <string>
#include <vector>
#include "hardware/uart.h"
#include "uart_instance.h"
#define RB_BUFFER_LENGTH 128

class DMABuffer {
    public:
        DMABuffer(uint uart_number);

    private:
        uint8_t buffer[RB_BUFFER_LENGTH];
};
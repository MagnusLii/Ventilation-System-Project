#pragma once

#include <memory>
#include "pico/time.h"
#include "uart_instance.h"

#define shared_modbus std::shared_ptr<ModbusCtrl>

class ModbusCtrl {
    public:
        ModbusCtrl(shared_uart uart_pointer);
        void start(void);
        void end(void);
        shared_uart uart_ptr;
    private:
        uint uart_baud;
        bool busy;
        uint32_t ctrl_time;
};
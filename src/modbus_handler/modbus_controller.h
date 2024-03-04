#pragma once

#include <memory>
#include "pico/time.h"
#include "uart_instance.h"
#include "dma_channel.h"


#define shared_modbus std::shared_ptr<ModbusCtrl>

class MODBUSRegister; // forward declaration need to prevent circular dependencies

class ModbusCtrl {
    public:
        ModbusCtrl(shared_uart uart_pointer);
        void start(uint8_t *payload, uint8_t paylen, uint8_t *rxbuf, uint8_t rxlen, MODBUSRegister *who);
        void end(void);
        bool isbusy(void);
    private:
        DMAChannel tx_channel;
        DMAChannel rx_channel;
        MODBUSRegister *current_user;
        shared_uart uart_ptr;
        uint uart_baud;
        volatile bool busy;
        uint64_t ctrl_time;
};
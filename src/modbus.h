#pragma once

#include "pico/stdlib.h"
#include "dma_buffer.h"


class Modbus {   
    public:
        Modbus();
        ~Modbus();

    private:
        uint8_t buffer[128];
};

Modbus::Modbus(/* args */)
{
}

Modbus::~Modbus()
{
}

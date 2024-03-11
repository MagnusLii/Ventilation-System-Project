#pragma once

#include "hardware/i2c.h"
#include "i2c_instance.h"

#define PAYLOAD_LEN 3

class PressureRegister {
    public:
        PressureRegister(shared_i2c i2c, uint8_t device_address);
        int get(uint32_t timeout_ms=500);
    private:
        shared_i2c i2c;
        uint8_t payload;
        uint8_t devaddr;
};

#pragma once

#include "hardware/i2c.h"

#define PAYLOAD_LEN 3

class PressureRegister {
    public:
        PressureRegister(uint i2c_index, uint8_t device_address);
        int get(uint32_t timeout_ms=500);
    private:
        i2c_inst_t *inst;
        uint8_t payload;
        uint8_t devaddr;
};

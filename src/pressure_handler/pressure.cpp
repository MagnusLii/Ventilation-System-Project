
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pressure.h"

#define READ_DIFF_CODE 0xF1


PressureRegister::PressureRegister(uint i2c_index, uint8_t device_address) {
    inst = i2c_index ? i2c1: i2c0;
    payload = READ_DIFF_CODE;
    devaddr = device_address;
}

int PressureRegister::get(uint32_t timeout_ms) {
    i2c_write_blocking(i2c1, devaddr, &payload, 1, false);
    uint8_t in[3];
    i2c_read_blocking_until(inst, devaddr, in, 3, false, delayed_by_ms(get_absolute_time(), timeout_ms));
    int value = in[0] << 8;
    value |= in[1];
    value = value / 240;
    return value;
}
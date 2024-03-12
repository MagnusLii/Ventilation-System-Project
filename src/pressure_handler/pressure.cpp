
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pressure.h"

#define READ_DIFF_CODE 0xF1


PressureRegister::PressureRegister(shared_i2c i2c, uint8_t device_address) : i2c(i2c) {
    payload = READ_DIFF_CODE;
    devaddr = device_address;
    // datasheet says one read after reset is not correct
    get();
}

int PressureRegister::get(uint32_t timeout_ms) {
    i2c_write_blocking(i2c->get_instance(), devaddr, &payload, 1, false);
    uint8_t in[3];
    i2c_read_blocking_until(i2c->get_instance(), devaddr, in, 3, false, delayed_by_ms(get_absolute_time(), timeout_ms));

    int16_t value = in[0] << 8;
    value |= in[1];
    value = (value * 95) / (240 * 100); // scale factor 240 and height compensation 0.95 (0 meters from sea)
    return value;
}
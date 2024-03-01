#pragma once

#include "pico/stdlib.h"

class GpioPin {
    friend void rotary_handler(uint gpio, uint32_t event_mask);
public:
    GpioPin(int pinNumber, bool isOutput = true, bool pullUp = false, bool isIrq = false);
    void setHigh();
    void setLow();
    void toggle();
    bool isHigh();
    bool isLow();
    int getPinNumber();

private:
    uint pinNumber;
};
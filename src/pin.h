#pragma once

#include "pico/stdlib.h"

class GpioPin {
public:
    GpioPin(int pinNumber, bool isOutput = true, bool pullUp = false);
    void setHigh();
    void setLow();
    void toggle();
    bool isHigh();
    bool isLow();
    int getPinNumber();

private:
    uint pinNumber;
};
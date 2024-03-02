#pragma once

#include "pico/stdlib.h"

class GpioPin {
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

class RotaryEncoder {
public:
    RotaryEncoder();
    int returnVal();
private:
    int rotValue;
    GpioPin Rot_A;
    GpioPin Rot_B;
};

class Handler {
public:
    static void rotHandler(uint gpio, uint32_t event);
};

class DataPass {
public:
    int data;
};
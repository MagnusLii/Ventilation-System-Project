#pragma once

#include "pico/stdlib.h"
#include "pin.h"
#include "pico/util/queue.h"

extern queue_t events;

class RotaryEncoder {
public:
    RotaryEncoder();
    int returnVal();
    void setVal(int val);
private:
    GpioPin Rot_A;
    GpioPin Rot_B;
    GpioPin Rot_SW;
};

class Button {
public:
    Button(int pin);
    bool returnState();
    int returnPin();
    void setState();
private:
    int pin;
};

class DataPass {
public:
    int data;
    int ButtonPin;
    bool buttonPressed;
};
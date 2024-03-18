#pragma once

#include "pico/stdlib.h"
#include "pin.h"
#include "pico/util/queue.h"

extern queue_t events;

class RotaryEncoder {
public:
    RotaryEncoder();
private:
    GpioPin Rot_A;
    GpioPin Rot_B;
    GpioPin Rot_SW;
};

class Button {
public:
    Button(int pin);
private:
    int pin;
};

#include "pico/stdlib.h"
#include "pin.h"
#include "stdio.h"

GpioPin::GpioPin(int pinNumber, bool isOutput, bool pullUp, bool isIrq) {
    this->pinNumber = pinNumber;
    gpio_init(pinNumber);
    if (isOutput) {
        gpio_set_dir(pinNumber, GPIO_OUT);
    } else {
        gpio_set_dir(pinNumber, GPIO_IN);
    }
    if (pullUp) {
        gpio_pull_up(pinNumber);
    }
    if (isIrq) {
        if (pinNumber == 10 || pinNumber == 11) {
            gpio_set_irq_enabled_with_callback(pinNumber, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &Handler::rotHandler);
        }
    }
}

void GpioPin::setHigh() {
    gpio_put(this->pinNumber, 1);
}

void GpioPin::setLow() {
    gpio_put(this->pinNumber, 0);
}

void GpioPin::toggle() {
    if (this->isHigh()) {
        this->setLow();
    } else {
        this->setHigh();
    }
}

bool GpioPin::isHigh() {
    return gpio_get(this->pinNumber);
}

bool GpioPin::isLow() {
    return !gpio_get(this->pinNumber);
}

int GpioPin::getPinNumber() {
    return this->pinNumber;
}

DataPass data;

RotaryEncoder::RotaryEncoder()
: Rot_A(10, false, false, true)
, Rot_B(11, false, false, true)
{}

int RotaryEncoder::returnVal() {
    return data.data;
}

void Handler::rotHandler(uint gpio, uint32_t event) {
    static int last_state = 0; // Static variable to store last known state of the rotary encoder
    int state = gpio_get(10); // Read current state of A pin

    if (last_state != state)
    {
        if (gpio_get(11) != state)
        {
            //Clockwise rotation increments in 10
            data.data += 10;
            if (data.data > 100) data.data = 100;
        }
        else
        {
            //Counter-clockwise rotation decrements in 10
            data.data -= 10;
            if (data.data < 0) data.data = 0;
        }
    }

    last_state = state; // Update last known state
}

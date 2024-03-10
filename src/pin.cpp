
#include "pico/stdlib.h"
#include "pin.h"
#include "input.h"

GpioPin::GpioPin(int pinNumber, bool isOutput, bool pullUp) {
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

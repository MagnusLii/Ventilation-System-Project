
#include "pico/stdlib.h"
#include "pin.h"
#include "stdio.h"

void rotary_handler(uint gpio, uint32_t event_mask) {
    if (gpio_get(10)) printf("1 \n");
    if (gpio_get(11)) printf("222 \n");
}

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
            gpio_set_irq_enabled_with_callback(pinNumber, GPIO_IRQ_EDGE_FALL, true, rotary_handler);
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
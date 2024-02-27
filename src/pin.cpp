
#include "pico/stdlib.h"


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
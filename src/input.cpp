#include "input.h"

DataPass data;

void handler(uint gpio, uint32_t event) {
    if (gpio == 10 || gpio == 11) {
        static int last_state = 0; // Static variable to store last known state of the rotary encoder
        int state = gpio_get(10); // Read current state of A pin

        if (last_state != state)
        {
            if (gpio_get(11) != state)
            {
                //Clockwise rotation increments in 1
                data.data += 1;
                if (data.data > 100) data.data -= 100;
            }
            else
            {
                //Counter-clockwise rotation decrements in 1
                data.data -= 1;
                if (data.data < 0) data.data += 100;
            }
        }
        last_state = state; // Update last known state
    } else if (gpio == 9 || gpio == 8 || gpio == 7 || gpio == 12) {
        data.ButtonPin = gpio;
        data.buttonPressed = true;
    }
}


RotaryEncoder::RotaryEncoder()
        : Rot_A(10, false, false)
        , Rot_B(11, false, false)
        , Rot_SW(12, false, true)
{
    gpio_set_irq_enabled_with_callback(Rot_A.getPinNumber(), GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, handler);
    gpio_set_irq_enabled_with_callback(Rot_B.getPinNumber(), GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, handler);
    gpio_set_irq_enabled_with_callback(Rot_SW.getPinNumber(), GPIO_IRQ_EDGE_RISE, true, handler);
}

int RotaryEncoder::returnVal() {
    return data.data;
}

Button::Button(int pin0) : pin(pin0) {
    GpioPin(pin, false, true);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, handler);
}

bool Button::returnState() {
    return data.buttonPressed;
}

int Button::returnPin() {
    return data.ButtonPin;
}

void Button::setState() {
    data.buttonPressed = false;
}

void RotaryEncoder::setVal(int val) {
    data.data = val;
}
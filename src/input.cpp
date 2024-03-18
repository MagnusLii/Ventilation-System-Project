#include "input.h"

//DataPass data;

void handler(uint gpio, uint32_t event) {
    int adjustment = 0;
    if (gpio == 10 || gpio == 11) {
        static int last_state = 0; // Static variable to store last known state of the rotary encoder

        int state = gpio_get(10); // Read current state of A pin

        if (last_state != state)
        {
            if (gpio_get(11) != state)
            {
                // Add clockwise rotation to queue
                adjustment = 1;
                queue_try_add(&events, &adjustment);
            }
            else
            {
                // Add counter-clockwise rotation to queue
                adjustment = -1;
                queue_try_add(&events, &adjustment);
            }
        }

        last_state = state; // Update last known state
    } else if (gpio == 9 || gpio == 8 || gpio == 7 || gpio == 12) {
        adjustment = 1000 + gpio;
        queue_try_add(&events, &adjustment);
    }
}

/*int RotaryEncoder::returnVal() {
    return data.data;
}*/

Button::Button(int pin0) : pin(pin0) {
    GpioPin(pin, false, true);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, handler);
}

/*bool Button::returnState() {
    return data.buttonPressed;
}

/*int Button::returnPin() {
    return data.ButtonPin;
}

/*void Button::setState() {
    data.buttonPressed = false;
}

void RotaryEncoder::setVal(int val) {
    data.data = val;
}*/
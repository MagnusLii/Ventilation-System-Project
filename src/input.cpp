#include "input.h"

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


RotaryEncoder::RotaryEncoder()
        : Rot_A(10, false, false)
        , Rot_B(11, false, false)
        , Rot_SW(12, false, true)
{
    gpio_set_irq_enabled_with_callback(Rot_A.getPinNumber(), GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, handler);
    gpio_set_irq_enabled_with_callback(Rot_B.getPinNumber(), GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, handler);
    gpio_set_irq_enabled_with_callback(Rot_SW.getPinNumber(), GPIO_IRQ_EDGE_RISE, true, handler);
}


Button::Button(int pin0) : pin(pin0) {
    GpioPin(pin, false, true);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, handler);
}
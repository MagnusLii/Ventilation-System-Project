#include "input.h"

#define N_IRQ_TRIGGERS 4
#define IRQ_MASK_MAX 8

static uint32_t time = 0;
static uint32_t delay = 50;

/**
 * Checks for a time delay since the last recorded time and updates it if the delay condition is met.
 *
 * @param time Pointer to the time value to be checked and updated.
 * @return true if the delay condition is met, otherwise false.
 */
bool debounce(uint32_t *time) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Get current time in milliseconds
    if (current_time - *time > delay) { // Check if the delay condition is met
        *time = current_time; // Update the time value to the current time
        return true; // Return true indicating the delay condition is met
    }
    return false; // Return false indicating the delay condition is not yet met
}

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
        if(debounce(&time)) {
            adjustment = 1000 + gpio;
            queue_try_add(&events, &adjustment);
        }
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
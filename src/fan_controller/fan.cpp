
#include <cmath>
#include "fan.h"

#define PRESSURE_TARGET_TOLERANCE 5
#define COUNTER_CHECK_RETRIES 2

#define PRESSURE_MIN 0
#define PRESSURE_MAX 150
#define SPEED_MIN 0
#define SPEED_MAX 1000

#define ERROR_DELAY 5000


FAN::FAN(WriteRegister *fan_speed_reg, ReadRegister *fan_counter_reg, PressureRegister *pressure, int speed) :
fan_speed_register(fan_speed_reg),
fan_counter_register(fan_counter_reg),
pressure_register(pressure),
speed(speed), pressure(0), counter(0) {
    fan_speed_reg->start_transfer((uint16_t)0);
} // damn this looks good

int FAN::pressure_distance(int target) {
    pressure = pressure_register->get();
    return target - pressure;
}

bool FAN::is_spinning() {
    for (int i=0; i<COUNTER_CHECK_RETRIES; i++) { // dunno if its possible but
        uint16_t old = counter;                   // incase the fan spins but the counter updates
        fan_counter_register->start_transfer();   // so fast it shows the same value
        fan_counter_register->wait();
        counter = fan_counter_register->get16();
        if (old != counter) return true;
    }
    return false;
}

void FAN::set_speed(uint16_t speed) {
    this->speed = speed;
    fan_speed_register->start_transfer(speed);
}

void FAN::adjust_speed(int target_pressure) {
    int distance = pressure_distance(target_pressure);
    
    if (abs(distance) < PRESSURE_TARGET_TOLERANCE) {
        error_time_ms = to_ms_since_boot(get_absolute_time());
        error = false;
        return;
    }
    uint32_t current = to_ms_since_boot(get_absolute_time());
    if ((current - error_time_ms) > ERROR_DELAY) {
        error = true;
    }
    float adjust = distance * SPEED_MAX / PRESSURE_MAX;
    speed += adjust / 10;
    if (speed > SPEED_MAX) speed = SPEED_MAX;
    if (speed < SPEED_MIN) speed = SPEED_MIN;
    fan_speed_register->start_transfer((uint16_t)speed);
    
}


int FAN::get_pressure(void) {
    return pressure;
}

int FAN::get_speed(void) {
    return speed;
}

bool FAN::get_error(void) {
    return error;
}
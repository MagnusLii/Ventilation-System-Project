#pragma once

#include "pressure.h"
#include "modbus_register.h"

class FAN {
    public:
        FAN(WriteRegister *fan_speed_reg, ReadRegister *fan_counter_register, PressureRegister *pressure, int speed=0);
        int pressure_distance(int target);
        bool is_spinning(void);
        void set_speed(uint16_t speed);
        void adjust_speed(int target_pressure);
        int get_pressure(void);
        int get_speed(void);
    private:
        WriteRegister *fan_speed_register;
        ReadRegister *fan_counter_register;
        PressureRegister *pressure_register;
        int speed;
        int pressure;
        uint16_t counter;
};
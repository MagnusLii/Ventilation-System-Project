#pragma once

#include "pin.h"
// #include "rotary.h"
// #include "screen.h"

typedef enum {
    WIFI,
    MQTT,
    MANUAL
} menu_stage;

class Menu {
private:
    GpioPin one;
    GpioPin two;
    GpioPin three;
    //Rotary rot;
    //Screen screen;
    menu_stage stage;
public:
    Menu();
    ~Menu();
    void update();
};



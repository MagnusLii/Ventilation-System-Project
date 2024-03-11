//
// Created by elmer on 6.3.2024.
//

#pragma once

#include "input.h"

void startMenu(ssd1306 &display, int pos);
void mqttMenu(ssd1306 &display, int pos);
void mainMenu(ssd1306 &display, Button &button, int pos, int fan_speed, int pressure, int target_pressure);

char* returnInput();
void textInput(ssd1306 &display, Button &button, int current_position);

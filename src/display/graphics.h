//
// Created by elmer on 6.3.2024.
//

#pragma once

#include "input.h"

void startMenu(ssd1306 &display, int pos);
void mainMenu(ssd1306 &display, int button, int *mode, int pos, int fan_speed, int pressure, int target_pressure, float temp, float co2, float rh, float ah);
void returnInput(char *str);
int textInput(ssd1306 &display, int button, int current_position, int stage);

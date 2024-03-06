#include "framebuf.h"
#include "ssd1306.h"
#include "pico/stdlib.h"
#include "mono_vlsb.h"
#include "graphics.h"


void mainMenu(ssd1306 &display) {
    display.fill(0);
    display.rect(9, 5, 110, 14, 1);
    display.text("SELECT SSID", 12, 8, 1);

    display.rect(9, 25, 110, 14, 1);
    display.text("USE PREVIOUS", 12, 28, 1);

    display.rect(9, 45, 110, 14, 1);
    display.text("NO WIFI", 12, 48, 1);
}

void mainMenuFirst(ssd1306 &display) {
    display.fill(0);
    display.rect(9, 5, 110, 14, 1, true);
    display.text("SELECT SSID", 12, 8, 0);

    display.rect(9, 25, 110, 14, 1, false);
    display.text("USE PREVIOUS", 12, 28, 1);

    display.rect(9, 45, 110, 14, 1, false);
    display.text("NO WIFI", 12, 48, 1);
}

void mainMenuSecond(ssd1306 &display) {
    display.fill(0);
    display.rect(9, 5, 110, 14, 1, false);
    display.text("SELECT SSID", 12, 8, 1);

    display.rect(9, 25, 110, 14, 1, true);
    display.text("USE PREVIOUS", 12, 28, 0);

    display.rect(9, 45, 110, 14, 1, false);
    display.text("NO WIFI", 12, 48, 1);
}

void mainMenuThird(ssd1306 &display) {
    display.fill(0);
    display.rect(9, 5, 110, 14, 1, false);
    display.text("SELECT SSID", 12, 8, 1);

    display.rect(9, 25, 110, 14, 1, false);
    display.text("USE PREVIOUS", 12, 28, 1);

    display.rect(9, 45, 110, 14, 1, true);
    display.text("NO WIFI", 12, 48, 0);
}
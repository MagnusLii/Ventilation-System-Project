#include "framebuf.h"
#include "ssd1306.h"
#include "pico/stdlib.h"
#include "mono_vlsb.h"
#include "graphics.h"

void set_function() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups
    i2c_init(i2c1, 400 * 1000);
}

void mainMenuNon() {
    set_function();
    ssd1306 mainMenu(i2c1);

    mainMenu.rect(9, 5, 110, 14, 1);
    mainMenu.text("SELECT SSID", 12, 8, 1);

    mainMenu.rect(9, 25, 110, 14, 1);
    mainMenu.text("USE PREVIOUS", 12, 28, 1);

    mainMenu.rect(9, 45, 110, 14, 1);
    mainMenu.text("NO WIFI", 12, 48, 1);

    mainMenu.show();
}

void mainMenuFirst() {
    set_function();
    ssd1306 mainMenu(i2c1);

    mainMenu.rect(9, 5, 110, 14, 1, true);
    mainMenu.text("SELECT SSID", 12, 8, 0);

    mainMenu.rect(9, 25, 110, 14, 1);
    mainMenu.text("USE PREVIOUS", 12, 28, 1);

    mainMenu.rect(9, 45, 110, 14, 1);
    mainMenu.text("NO WIFI", 12, 48, 1);

    mainMenu.show();
}

void mainMenuSecond() {
    set_function();
    ssd1306 mainMenu(i2c1);

    mainMenu.rect(9, 5, 110, 14, 1);
    mainMenu.text("SELECT SSID", 12, 8, 1);

    mainMenu.rect(9, 25, 110, 14, 1, true);
    mainMenu.text("USE PREVIOUS", 12, 28, 0);

    mainMenu.rect(9, 45, 110, 14, 1);
    mainMenu.text("NO WIFI", 12, 48, 1);

    mainMenu.show();
}

void mainMenuThird() {
    set_function();
    ssd1306 mainMenu(i2c1);

    mainMenu.rect(9, 5, 110, 14, 1);
    mainMenu.text("SELECT SSID", 12, 8, 1);

    mainMenu.rect(9, 25, 110, 14, 1);
    mainMenu.text("USE PREVIOUS", 12, 28, 1);

    mainMenu.rect(9, 45, 110, 14, 1, true);
    mainMenu.text("NO WIFI", 12, 48, 0);

    mainMenu.show();
}
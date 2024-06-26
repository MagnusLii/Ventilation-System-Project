#include "ssd1306.h"
#include "graphics.h"
#include "input.h"
#include "string.h"

void startMenu(ssd1306 &display, int pos) {
    display.fill(0);
    if (pos == 0 || pos == 1) {
        display.rect(9, 5, 110, 14, 1, true);
        display.text("SELECT SSID", 12, 8, 0);
    } else {
        display.rect(9, 5, 110, 14, 1);
        display.text("SELECT SSID", 12, 8, 1);
    }

    if (pos == 2 || pos == 3) {
        display.rect(9, 25, 110, 14, 1, true);
        display.text("USE PREVIOUS", 12, 28, 0);
    } else {
        display.rect(9, 25, 110, 14, 1);
        display.text("USE PREVIOUS", 12, 28, 1);
    }
    if ((pos == 4 || pos == 5)) {
        display.rect(9, 45, 110, 14, 1, true);
        display.text("NO WIFI", 12, 48, 0);
    } else {
        display.rect(9, 45, 110, 14, 1);
        display.text("NO WIFI", 12, 48, 1);
    }
}

static char alphabetsCap[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
static char alphabets[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
static char characters[31] = {'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@',
                     '[', ']', '^', '_', '`', '{', '|', '}', '~'};
static char numbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
static char* arrays[] = {alphabetsCap, alphabets, characters, numbers};

static int current_Array = 0;
static char input_string[60] = {0};

void returnInput(char *str) {
    strcpy(str, input_string);
    str[60] = '\0';
    memset(input_string, '\0', sizeof(input_string));
}

void textInput(ssd1306 &display, Button &button, int current_position, int stage) {
    std::string toxt;
    switch (stage) {
    case 0:
        toxt = "SSID:";
        break;
    case 1:
        toxt = "PASSWORD:";
        break;
    case 2:
        toxt = "HOST:";
        break;
    case 3:
        current_Array = 3;
        toxt = "PORT:";
        break;
    default:
        break;
    }
    char c[2] = {0, 0};
    int arr_length;
    
    switch (current_Array) { //determine which array to use and set the length to a variable
        case 0:
            arr_length = sizeof(alphabetsCap);
            break;
        case 1:
            arr_length = sizeof(alphabets);
            break;
        case 2:
            arr_length = sizeof(characters);
            break;
        case 3:
            arr_length = sizeof(numbers);
            break;
    }
    int pos = current_position % arr_length;
    int input_len = strlen(input_string);

    if (button.returnState()) { //do stuff when a defined button is pressed
        button.setState();
        switch (button.returnPin()) {
            case 9: // switch between the character arrays
                current_Array++;
                if (current_Array > 3) current_Array = 0;
                break;
            case 8: // write characters to input string
                if (input_len < 60) {
                    c[0] = arrays[current_Array][pos];
                    strcat(input_string, c);
                }
                break;
            case 7: // delete characters from input string
                if (input_len > 0) input_string[input_len - 1] = '\0';
        }
    }
    display.fill(0);
    display.text(toxt, 0, 0, 1);
    display.rect(112, 4, 14, 56, 1);
    display.rect(0, 22, 110, 14, 1);
    display.text(input_string, 108 - (input_len * 8), 26);

    if (pos - 1 < 0) {
        c[0] = arrays[current_Array][arr_length - 1];
    } else c[0] = arrays[current_Array][pos - 1];
    display.text(c, 114, 10, 1);

    c[0] = arrays[current_Array][pos];
    display.rect(114, 25, 9, 9, 1, true);
    display.text(c, 114, 26, 0);

    int pos1 = pos + 1;
    if (pos1 >= arr_length) {
        c[0] = arrays[current_Array][0];
    } else {
        c[0] = arrays[current_Array][pos + 1];
    }
    display.text(c, 114, 42, 1);
}

int display_mode = 1;
void mainMenu(ssd1306 &display, Button &button, int *mode, int pos, int fan_speed, int pressure, int target_pressure, float temp, float co2, float rh, float ah) {
    char output_string[30];

    //temp, c02, rh, ah
    if (button.returnState() && button.returnPin() == 8) {
        button.setState();
        if (display_mode == 0) display_mode = 1;
        else display_mode = 0;
    }

    if (display_mode == 0) {
        display.fill(0);
        snprintf(output_string, sizeof(output_string), "temp: %d", (int)temp);
        display.text(output_string, 2, 10, 1);
        snprintf(output_string, sizeof(output_string), "co2: %d", (int)co2);
        display.text(output_string, 2, 20, 1);
        snprintf(output_string, sizeof(output_string), "rh: %d", (int)rh);
        display.text(output_string, 2, 30, 1);
        snprintf(output_string, sizeof(output_string), "ah: %d", (int)ah);
        display.text(output_string, 2, 40, 1);
        //display_mode = 1;
    } else if(display_mode == 1) {

        //change between manual and auto mode
        if (button.returnState() && button.returnPin() == 9) {
            button.setState();
            *mode = !*mode;
        }


            if (*mode == 1)
            {
                display.fill(0);
                display.rect(4, 4, 58, 20, 1, true);
                display.rect(66, 4, 58, 20, 1);
                display.text("MANUAL", 10, 10, 0);
                display.text("AUTO", 80, 10, 1);
            } else {
                display.fill(0);
                display.rect(4, 4, 58, 20, 1);
                display.rect(66, 4, 58, 20, 1, true);
                display.text("MANUAL", 10, 10, 1);
                display.text("AUTO", 80, 10, 0);
            }

        if (*mode == 1) {
            sniprintf(output_string, sizeof(output_string), "fanspeed: %d", fan_speed);
            display.text(output_string, 4, 30, 1);
        } else {
            sniprintf(output_string, sizeof(output_string), "pressure: %d", pressure);
            display.text(output_string, 2, 30, 1);
            sniprintf(output_string, sizeof(output_string), "%d", target_pressure);
            display.text("target pressure: ", 2, 40, 1);
            display.text(output_string, 2, 50, 1);
        }
    }

}
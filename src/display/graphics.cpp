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

void mqttMenu(ssd1306 &display, int pos) {
    display.fill(0);
    if (pos == 0 || pos == 1) {
        display.rect(9, 5, 110, 14, 1, true);
        display.text("SELECT MQTT", 12, 8, 0);
    } else {
        display.rect(9, 5, 110, 14, 1);
        display.text("SELECT MQTT", 12, 8, 1);
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
        display.text("NO MQTT", 12, 48, 0);
    } else {
        display.rect(9, 45, 110, 14, 1);
        display.text("NO MQTT", 12, 48, 1);
    }
}

char alphabetsCap[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
char alphabets[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
char characters[31] = {'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@',
                     '[', ']', '^', '_', '`', '{', '|', '}', '~'};
char* arrays[] = {alphabetsCap, alphabets, characters};

int current_Array = 0;
char input_string[60];
char returnString[60];

char* returnInput() {
    strcpy(returnString, input_string);
    memset(input_string, '\0', sizeof(input_string));
    return returnString;
}

void textInput(ssd1306 &display, Button &button, int current_position) {
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
    }
    int pos = current_position % arr_length;
    int input_len = strlen(input_string);

    if (button.returnState()) { //do stuff when a defined button is pressed
        button.setState();
        switch (button.returnPin()) {
            case 9: // switch between the character arrays
                current_Array++;
                if (current_Array > 2) current_Array = 0;
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
#include <algorithm>
#include "logHandler.h"
#include "pico/stdlib.h"

#include "uart_instance.h"
#include "i2c_instance.h"

#include "modbus_controller.h"
#include "modbus_register.h"
#include "pressure.h"
#include "fan.h"
#include "eeprom.h"
#include <iostream>
#include <string>
#include <vector>
#include "hardware/timer.h"
#include "pico/time.h"
#include "commhandler.h"
#include <stdlib.h>

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

#define DEBUG_ENABLE
#include "debugprint.h"
#include "pin.h"
#include "input.h"
#include "display/ssd1306.h"
#include "display/graphics.h"
#include "display/font_petme128_8x8.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define I2C1_SDA 14
#define I2C1_SCL 15
#define I2C1_BAUD 400000

#define SEND_DELAY 5000

#define DEFAULT_HOSTNAME "192.168.1.10"
#define DEFAULT_PORT 1883
#define DEFAULT_SSID "SmartIotMQTT"
#define DEFAULT_PW "SmartIot"


bool user_input(char *dst, int size) {
    if (uart_is_readable(uart0)) {
        char c = 0;
        int i = 0;
        while (c != '\n' && i < size) {
            c = fgetc(stdin);
            dst[i] = c;
            i++;
        }
        dst[--i] = '\0';
        return true;
    }
    return false;
}
#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5
    
int main() {
    stdio_init_all();

    //init buttons and rotary encoder
    Button button(7);
    Button button1(8);
    Button button2(9);
    RotaryEncoder Rotary;

    // BOILERPLATE
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN, 1)}; //1 for testbox 2 for fullscale
    shared_i2c i2c{std::make_shared<I2C_instance>(i2c1, I2C1_BAUD, I2C1_SDA, I2C1_SCL)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};


    ssd1306 display(i2c1);

    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);
    LogHandler logHandler;

    // logHandler.pushLog(BOOT);

    //char ssid[64] = DEFAULT_SSID;
    char ssid[64] = "";
    //char pw[64]  = DEFAULT_PW;
    char pw[64] = "";
    //char hostname[64] = DEFAULT_HOSTNAME;
    char hostname[64] = "";
    //int port = DEFAULT_PORT;
    int port = 0;
    int verificationArray[4] = {0, 0, 0, 0};

    //logHandler.storeCredentials(ssid, pw, hostname, port);

    logHandler.fetchCredentials(ssid, pw, hostname, &port, verificationArray);

    std::cout << "VerifyBits: ";
    for (int i = 0; i < 4; i++) {
        std::cout << verificationArray[i];
    }
    std::cout << std::endl;

    std::cout << "SSID: " << ssid << std::endl;
    std::cout << "PW: " << pw << std::endl;
    std::cout << "Hostname: " << hostname << std::endl;
    std::cout << "Port: " << port << std::endl;

    // TODO MENU
    // WIFI MENU
    // TEXT MENU
    bool done = false;
    int stage = 0, ssid_stage = 0, rotvalue = 0;
    std::string input;
    while (done == false)
    {   
        if (stage == 0)
        {
            rotvalue = Rotary.returnVal() % 6;
            startMenu(display, rotvalue);
            display.show();
        } else if(stage == 1) 
        {
            textInput(display, button, Rotary.returnVal());
            display.show();

            if (button.returnPin() == 7 && button.returnState() == true)
            {
                button.setState();
                input = returnInput();
                if (input.empty())
                {
                    stage = 0;
                }
                
            } else if(button.returnPin() == 12 && button.returnState() == true) {
                button.setState();
                if (ssid_stage < 4)
                {
                    switch (ssid_stage)
                    {
                    case 0:
                        ssid = returnInput();
                        break;
                    case 1:
                        pw = returnInput();
                        break;
                    case 2:
                        hostname = returnInput();
                        break;
                    case 3:
                        port = std::stoi(returnInput());
                        break;
                    default:
                        break;
                    }
                    ssid_stage++;
                } else done = true;
            }
        }
        
        if (button.returnPin() == 9 && button.returnState() == true && stage == 0)
        {
            button.setState();
            switch (rotvalue)
            {
            case 0://select ssid, pw, hostname and port
            case 1:
                stage = 1;
                break;
            case 2:// use previous settings
            case 3:
                //use previous settings and proceed
                done = true;
                break;
            case 4:// proceed with no wifi??
            case 5:
                //possibly just go straight to mainMenu??
                //disable wifi or something i dont really know
                done = true;
                break;
            default:
                rotvalue = 0;
                break;
            }
        }
    }
    
    IPStack ipstack(ssid, pw);
    if (ipstack.get_success() == 0) {
        DPRINT("Indeed failed\n");
        while (true) tight_loop_contents();
    }
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    CommHandler comm_handler(ipstack, client);

    comm_handler.connect_to_server(hostname, port);
    comm_handler.connect_to_broker();
    comm_handler.subscribe(CONTROL_TOPIC);
    // TODO log successful jotain


    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister absh(mbctrl, 241, 0xe, 2);
    ReadRegister temp(mbctrl, 240, 4, 2);
    ReadRegister co(mbctrl, 240, 0, 2);

    WriteRegister fan_speed(mbctrl, 1, 0, 1);
    ReadRegister fan_counter(mbctrl, 1, 4, 1, false);
    PressureRegister pre(i2c, 64);
    FAN fan(&fan_speed, &fan_counter, &pre);

    uint32_t send_time = 0;
    int old_set_point = get_set_point();
    while (1) {
        // STATUS MENU
        mainMenu(display, button, Rotary.returnVal(), fan.get_speed() / 10, fan.get_pressure(),
                get_set_point(), temp.get_float(),
                co.get_float(), rh.get_float(), absh.get_float());
        display.show();

        if (get_manual()) {
            if (old_set_point != get_set_point()) {
                Rotary.setVal(get_set_point());
                old_set_point = get_set_point();
            }
            fan.set_speed(Rotary.returnVal() * 10); // target = speed in percentage
        } else {
            fan.adjust_speed(get_set_point());
            // TODO LOG ERROR fan.get_error();
        }
        uint32_t new_time = to_ms_since_boot(get_absolute_time());
        if ((new_time - send_time) > SEND_DELAY) {
            send_time = new_time;
            rh.start_transfer();
            absh.start_transfer();
            co.start_transfer();
            temp.start_transfer();
            while (mbctrl->isbusy()) tight_loop_contents(); // IMPORTANT if modbus controller is busy it means the transfers are not compeleted yet
            // this should send mqtt message
            comm_handler.send(fan.get_speed() / 10, get_set_point(), fan.get_pressure(),
                                get_manual(), fan.get_error(), co.get_float(), absh.get_float(),
                                rh.get_float(), temp.get_float());    
        }

        client.yield(100);
    }
}

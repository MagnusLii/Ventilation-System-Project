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
#include "pico/util/queue.h"
#include <queue>

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
#define ADJUST_DELAY 5000

#define DEFAULT_HOSTNAME "192.168.55.236"
#define DEFAULT_PORT 1883
#define DEFAULT_SSID "Mgzz-57"
#define DEFAULT_PW "557949122aA"

#define EEPROM_BAUD_RATE 1000000
#define EEPROM_WRITE_CYCLE_MAX_MS 5

 queue_t events;

int main()
{
    bool use_wifi = false;
    stdio_init_all();

    // init buttons and rotary encoder
    Button button(7);
    Button button1(8);
    Button button2(9);
    RotaryEncoder Rotary;



    // BOILERPLATE
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN, 1)}; // 1 for testbox 2 for fullscale
    shared_i2c i2c{std::make_shared<I2C_instance>(i2c1, I2C1_BAUD, I2C1_SDA, I2C1_SCL)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};

    ssd1306 display(i2c1);

    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);


    int event_result;
    int rotvalue = 0;
    int button_pressed = 0;
    std::shared_ptr<IPStack> ipstackptr;
    std::shared_ptr<CommHandler> commHandlerptr;
    std::shared_ptr<MQTT::Client<IPStack, Countdown>> clientptr;

    bool netword_done = false;

    LogHandler logHandler;

    // logHandler.pushLog(BOOT);

    // CHANGE THESE
    char ssid[64] = {0};
    char pw[64] = {0};
    char hostname[64] = {0};
    int port = 0;
    use_wifi = true;
    int verArray[4] = {0, 0, 0, 0};

    logHandler.fetchCredentials(ssid, pw, hostname, &port,  verArray);

    queue_init(&events, sizeof(int), 50);
    while (!netword_done)
    {
        bool done = false;
        int stage = 0;
        char input[64];

        while (!done)
        {
            if (stage == 0)
            {
                startMenu(display, rotvalue);
                display.show();
            }
            else if (stage >= 1)
            {
                textInput(display, button_pressed, rotvalue, stage);
                display.show();

                if (button_pressed == 7)
                {
                    returnInput(input);
                    if (strlen(input) == 0)
                    {
                        stage--;
                        if (stage < 0) stage = 0;
                    }
                }
                else if (button_pressed == 12)
                {
                    if (stage < 6 && stage >= 1)
                    {
                        switch (stage)
                        {
                        case 1:
                            returnInput(ssid);
                            break;
                        case 2:
                            returnInput(pw);
                            break;
                        case 3:
                            returnInput(hostname);
                            break;
                        case 4:
                            char input_port[64];
                            returnInput(input_port);
                            port = std::stoi(input_port);
                            break;
                        default:
                            break;
                        }
                        stage++;
                    }
                    else
                        done = true;
                }
            }

            if (button_pressed == 9 && stage == 0)
            {
                switch (rotvalue)
                {
                case 1:
                    stage = 1;
                    break;
                case 2:
                    // use previous settings and proceed
                    done = true;
                    break;
                case 3: // proceed with no wifi?
                    // possibly just go straight to mainMenu??
                    // disable wifi or something i dont really know
                    use_wifi = false;
                    netword_done = true;
                    done = true;
                    break;
                default:
                    break;
                }
            }
            button_pressed = 0;
            if(queue_try_remove(&events, &event_result) == true)
            {
                if (event_result > 1000) {
                    button_pressed = event_result - 1000;
                } else rotvalue += event_result;

                if (stage == 0) {
                    if (rotvalue < 1) rotvalue = 3;
                    else if (rotvalue > 3) rotvalue = 1;
                }
            }
        }
        if (use_wifi)
        {
            ipstackptr = std::make_shared<IPStack>(ssid, pw);

            if (ipstackptr->get_success())
            {
                netword_done = true;
            }
            clientptr = std::make_shared<MQTT::Client<IPStack, Countdown>>(*ipstackptr);
            CommHandler comm_handler(*ipstackptr, *clientptr);
            commHandlerptr = std::make_shared<CommHandler>(*ipstackptr, *clientptr);
            comm_handler.connect_to_server(hostname, port);
            comm_handler.connect_to_broker();
            comm_handler.subscribe(CONTROL_TOPIC);
            logHandler.setCommHandler(commHandlerptr);
            if (!comm_handler.verify_connection())
            {
                netword_done = false;
            } else {
                DPRINT("logged stuff");
                logHandler.storeCredentials(ssid, pw, hostname, port);
            }
        }
        // TODO log successful jotain
    }

    ReadRegister rh(mbctrl, 241, 0, 2);
    ReadRegister absh(mbctrl, 241, 0xe, 2);
    ReadRegister temp(mbctrl, 240, 4, 2);
    ReadRegister co(mbctrl, 240, 0, 2);

    WriteRegister fan_speed(mbctrl, 1, 0, 1);
    ReadRegister fan_counter(mbctrl, 1, 4, 1, false);
    PressureRegister pre(i2c, 64);
    FAN fan(&fan_speed, &fan_counter, &pre);

    uint32_t send_time = 0;
    uint32_t adjust_time = 0;
    uint32_t new_time = 0;
    int old_set_point = get_set_point();
    int fan_speed_val = 0;
    int pressure_val = 0;

    while (1)
    {
        // STATUS MENU
        int mode = (int)get_manual();
        mainMenu(display, button_pressed, &mode, rotvalue, fan.get_speed() / 10, fan.get_pressure(),
                 pressure_val, temp.get_float(),
                 co.get_float(), rh.get_float(), absh.get_float());
        display.show();
        set_manual(mode);

        logHandler.pushLog(TEST, use_wifi);

        if (get_manual())
        {
            if (get_set_point() != old_set_point)
            {
                old_set_point = get_set_point();
                fan_speed_val = get_set_point();
            }
            fan.set_speed(fan_speed_val * 10); // target = speed in percentage
        }
        else
        {
            if (get_set_point() != old_set_point)
            {
                old_set_point = get_set_point();
                pressure_val = get_set_point();
            }
            new_time = to_ms_since_boot(get_absolute_time());
            if ((new_time - adjust_time) > ADJUST_DELAY) {
                adjust_time = new_time;
                fan.adjust_speed(pressure_val);
            }
        
            // TODO LOG ERROR fan.get_error();
        }
        new_time = to_ms_since_boot(get_absolute_time());
        if ((new_time - send_time) > SEND_DELAY)
        {
            send_time = new_time;
            rh.start_transfer();
            absh.start_transfer();
            co.start_transfer();
            temp.start_transfer();
            while (mbctrl->isbusy())
                tight_loop_contents(); // IMPORTANT if modbus controller is busy it means the transfers are not compeleted yet
            // this should send mqtt message
            if (use_wifi)
            {
                commHandlerptr->send(fan.get_speed() / 10, (mode) ? fan_speed_val : pressure_val, fan.get_pressure(),
                                     !get_manual(), fan.get_error(), co.get_float(), absh.get_float(),
                                     rh.get_float(), temp.get_float());
            }
        }
        if (use_wifi)
        {
            clientptr->yield(100);
        }
        button_pressed = 0;
        if(queue_try_remove(&events, &event_result))
        {
            if (event_result > 1000) {
                button_pressed = event_result - 1000;
            }

            if (get_manual() && event_result < 1000) {
                fan_speed_val += event_result;
            } else if(!get_manual() && event_result < 1000){
                pressure_val += event_result;
            }

            if (fan_speed_val < 0) fan_speed_val = 0;
            if (fan_speed_val > 100) fan_speed_val = 100;

            if (pressure_val < 0) pressure_val = 0;
            if (pressure_val > 120) pressure_val = 120;
        }
    }
}

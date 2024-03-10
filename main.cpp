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


#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define I2C1_SDA 14
#define I2C1_SCL 15
#define I2C1_BAUD 100000


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

    // BOILERPLATE
    shared_uart u{std::make_shared<Uart_instance>(1, 9600, UART_TX_PIN, UART_RX_PIN)};
    shared_i2c i2c{std::make_shared<I2C_instance>(i2c1, I2C1_BAUD, I2C1_SDA, I2C1_SCL)};
    shared_modbus mbctrl{std::make_shared<ModbusCtrl>(u)};

    eeprom_init_i2c(i2c0, EEPROM_BAUD_RATE, EEPROM_WRITE_CYCLE_MAX_MS);
    LogHandler logHandler;

    // logHandler.pushLog(BOOT);

    // TODO MENU

    const char *ssid = "SSID";
    const char *pw = "PW";
    const char *hostname = "0.0.0.0";
    int port = 1883;

    IPStack ipstack(ssid, pw);
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    CommHandler comm_handler(ipstack, client);

    comm_handler.connect_to_server(hostname, port);
    comm_handler.connect_to_broker();
    comm_handler.subscribe(DATA_TOPIC);
    // TODO log successful jotain


    ReadRegister rh(mbctrl, 241, 256);
    ReadRegister absh(mbctrl, 241, 0xe, 2);
    ReadRegister temp(mbctrl, 240, 4, 2);
    ReadRegister co(mbctrl, 240, 0, 2);

    WriteRegister fan_speed(mbctrl, 1, 0, 1);
    ReadRegister fan_counter(mbctrl, 1, 4, 1, false);
    PressureRegister pre(i2c, 64);
    FAN fan(&fan_speed, &fan_counter, &pre);

    char buf[20];
    int target = 50;
    bool spinning = false;
    bool manual = false;
    while (1) {
        if (get_manual()) {
            fan.set_speed(get_set_point() * 10); // target = speed in percentage
        } else {
            fan.adjust_speed(get_set_point());
        }

// {
// "nr": 96,
// "speed": 18,
// "setpoint": 18,
// "pressure": 5,
// "auto": false,
// "error": false,
// "co2": 300,
// "ah": 200,
// "rh": 37,
// "temp": 20 
// }
        rh.start_transfer();
        absh.start_transfer();
        temp.start_transfer();
        co.start_transfer();
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        while(mbctrl->isbusy()) tight_loop_contents();
        writer.StartObject();
        writer.Key("speed");
        writer.Int(fan.get_speed() / 10);
        writer.Key("setpoint");
        writer.Int(get_set_point());
        writer.Key("pressure");
        writer.Int(fan.get_pressure());
        writer.Key("auto");
        writer.Bool(!get_manual());
        writer.Key("error");
        writer.Bool(false);
        writer.Key("co2");
        writer.Double(co.get_float());
        writer.Key("ah");
        writer.Double(absh.get_float());
        writer.Key("rh");
        writer.Double(rh.get_float());
        writer.Key("temp");
        writer.Double(temp.get_float());
        writer.EndObject();

        const char* jsonString = s.GetString();
        

    }
}

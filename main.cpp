#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "pico/stdlib.h"

#include "uart_instance.h"
#include "modbus.h"

#define DEBUG_ENABLE
#include "debugprint.h"


int main() {
    stdio_init_all();
    shared_uart u = std::make_shared<Uart_instance>(0, 9600, )
    ReadRegister co2reg()

    return 0;
}
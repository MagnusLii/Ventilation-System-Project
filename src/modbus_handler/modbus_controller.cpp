
#include "pico/time.h"
#include "modbus_controller.h"


static ModbusCtrl *point;
void dma_irq_handler(void) {
    point->end();
}

ModbusCtrl::ModbusCtrl(shared_uart uart_pointer) :
    tx_channel(uart_pointer, true),
    rx_channel(uart_pointer, false),
    uart_ptr(uart_pointer),
    uart_baud(uart_pointer->get_baud()),
    busy(false),
    ctrl_time(0) {
        irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
        irq_set_enabled(DMA_IRQ_0, true);
        point = this;
    }


static inline uint64_t get_char_delay(uint baud) {
    // 1/baud = time it takes to send one bit
    // bit time * 8 = time it takes to send one character (8 bits)
    // byte time * 1000000 = byte time in microseconds
    // byte time us * 4 = time it takes to send 4 characters in microseconds
    return (8*1000000*4)/baud;
}

void ModbusCtrl::start(uint8_t *payload, uint8_t paylen, uint8_t *rxbuf, uint8_t rxlen) {
    // if busy and not enough delay for next transfer wait
    while(busy && ((time_us_64() - ctrl_time) < get_char_delay(uart_baud))) tight_loop_contents();
    rx_channel.start(rxbuf, rxlen);
    tx_channel.start(payload, paylen);
    busy = true;
}

void ModbusCtrl::end(void) {
    ctrl_time = time_us_64();
    busy = false;
}
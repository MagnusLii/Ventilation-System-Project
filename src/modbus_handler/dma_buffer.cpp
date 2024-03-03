#include <string>
#include <vector>
#include <memory>
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "uart_instance.h"
#include "modbus_register.h"
#include "dma_buffer.h"

#define MAX_DMA_CHANNELS 12 // from datasheet

static DMABuffer *active_channels[MAX_DMA_CHANNELS] = {nullptr};
static bool irq0_enabled = false;

void dma_irq_handler(void) {
    for (int i=0; i<MAX_DMA_CHANNELS; i++) {
        if (active_channels[i]) {
            if (dma_channel_get_irq0_status(i)) {
                dma_channel_acknowledge_irq0(i);
                active_channels[i]->irq_hand();
            }
        }
    }
}

DMABuffer::DMABuffer(shared_uart uart_pointer, MODBUSRegister *owner) :
uart(uart_pointer), owner(owner) {
    dma_channel = dma_claim_unused_channel(true);
    active_channels[dma_channel] = this;
    if (!irq0_enabled) {
        irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
        irq_set_enabled(DMA_IRQ_0, true);
        irq0_enabled = true;
    }
}

void DMABuffer::irq_hand() {
    owner->done();
}


DMABuffer::~DMABuffer() {
    dma_channel_cleanup(dma_channel);
    dma_channel_unclaim(dma_channel);
    active_channels[dma_channel] = nullptr;
}


DMARXBuffer::DMARXBuffer(shared_uart uart_pointer, MODBUSRegister *owner) :
DMABuffer(uart_pointer, owner) {
    dma_channel_config c = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8); // uart deals with 8 bit characters
    channel_config_set_read_increment(&c, false); // read from same fifo so no increment
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, (uart->get_index()) ? DREQ_UART1_RX : DREQ_UART0_RX);
    dma_channel_configure(
        dma_channel,
        &c,
        buffer,
        uart->get_dr_address(),
        0,
        false
    );
    dma_channel_set_irq0_enabled(dma_channel, true);
}


void DMARXBuffer::start_listening(uint8_t max_characters) {
    dma_channel_set_write_addr(dma_channel, &buffer, false);
    dma_channel_set_trans_count(dma_channel, max_characters, true);
}


DMATXBuffer::DMATXBuffer(shared_uart uart_pointer, uint8_t *buffer, MODBUSRegister *owner) :
DMABuffer(uart_pointer, owner), buffer(buffer) {
    dma_channel_config c = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8); // uart deals with 8 bit characters
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false); // always write to uart tx fifo
    channel_config_set_dreq(&c, (uart->get_index()) ? DREQ_UART1_TX : DREQ_UART0_TX);
    dma_channel_configure(
        dma_channel,
        &c,
        uart->get_dr_address(),
        buffer,
        0,
        false
    );
}

void DMATXBuffer::set_tx_buffer(uint8_t *buffer) {
    this->buffer = buffer;
    dma_channel_set_read_addr(dma_channel, buffer, false);
}

void DMATXBuffer::set_transfer_in_flight(uint8_t transfer_count) {
    dma_channel_set_read_addr(dma_channel, buffer, false);
    dma_channel_set_trans_count(dma_channel, transfer_count, true);
}
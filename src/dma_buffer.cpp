#include <string>
#include <vector>
#include <memory>
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "uart_instance.h"
#include "dma_buffer.h"


DMABuffer::DMABuffer(std::shared_ptr<Uart_instance> uart_pointer) :
uart(uart_pointer) {
    dma_channel = dma_claim_unused_channel(true);
}

DMABuffer::~DMABuffer() {
    dma_channel_cleanup(dma_channel);
    dma_channel_unclaim(dma_channel);
}

DMARXBuffer::DMARXBuffer(std::shared_ptr<Uart_instance> uart_pointer) :
DMABuffer(uart_pointer) {
    // dma_sniffer_enable(dma_channel, 0x2, true); // sniff for CRC-16-CCITT

    dma_channel_config c = dma_get_channel_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8); // uart deals with 8 bit characters
    channel_config_set_read_increment(&c, false); // read from same fifo so no increment
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, (uart->get_index()) ? DREQ_UART1_RX : DREQ_UART0_RX);
    channel_config_set_sniff_enable(&c, true); // sniff for crc
    dma_channel_configure(
        dma_channel,
        &c,
        buffer,
        uart->get_dr_address(),
        0,
        false
    );
}

DMATXBuffer::DMATXBuffer(std::shared_ptr<Uart_instance> uart_pointer, uint8_t *buffer) :
DMABuffer(uart_pointer), buffer(buffer) {
    dma_channel_config c = dma_get_channel_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8); // uart deals with 8 bit characters
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false); // always write to uart tx fifo
    channel_config_set_dreq(&c, (uart->get_index()) ? DREQ_UART1_RX : DREQ_UART0_RX);
    dma_channel_configure(
        dma_channel,
        &c,
        uart->get_dr_address(),
        buffer,
        0,
        false
    );
}

void set_tx_buffer(uint8_t *buffer) {
    this->buffer = buffer;
    dma_channel_set_read_addr(dma_channel, buffer, false);
}
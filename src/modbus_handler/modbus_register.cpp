#include "pico/stdlib.h"

#include "modbus_register.h"

#define FUNC_READ_HOLDING_REGISTER 0x3
#define FUNC_READ_INPUT_REGISTERS 0x4
#define FUNC_WRITE_MULTIPLE_REGISTERS 0x10

#define PAYLOAD_HEADER_LEN 6

#define INDEX_DEVADDR 0
#define INDEX_FUNC 1
#define INDEX_REGADDR_MSB 2
#define INDEX_REGADDR_LSB 3
#define INDEX_REG_COUNT_MSB 4
#define INDEX_REG_COUNT_LSB 5
#define INDEX_READ_CRC_LSB 6
#define INDEX_READ_CRC_MSB 7

#define INDEX_ERROR_CODE 2

#define INDEX_DATA_BYTE_COUNT 6
#define INDEX_DATA_LSB 8
#define INDEX_DATA_MSB 7
#define INDEX_WRITE_CRC_LSB 9
#define INDEX_WRITE_CRC_MSB 10

#define READ_EXPECTED_CHARACTERS 5
#define WRITE_EXPECTED_CHARACTERS 8


/*
This function is shamelessly copied from nanomodbus library
*/
static uint16_t crc(const uint8_t* data, uint32_t length) {
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < length; i++) {
        crc ^= (uint16_t) data[i];
        for (int j = 8; j != 0; j--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }
    return (uint16_t) (crc << 8) | (uint16_t) (crc >> 8);
}

MODBUSRegister::MODBUSRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address, uint8_t number_of_registers) :
    modbus(modbus), number_of_registers(number_of_registers) {
    payload[INDEX_DEVADDR] = device_address;
    // func index goes between here
    payload[INDEX_REGADDR_MSB] = (uint8_t)(register_address >> 8);
    payload[INDEX_REGADDR_LSB] = (uint8_t)register_address;
    payload[INDEX_REG_COUNT_MSB] = 0; // not doing that many registers at once
    payload[INDEX_REG_COUNT_LSB] = number_of_registers; // up to 255 registers
    payload_len = PAYLOAD_HEADER_LEN;   
}


ReadRegister::ReadRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address, uint8_t number_of_registers, bool holding) :
MODBUSRegister(modbus, device_address, register_address, number_of_registers) {
    payload[INDEX_FUNC] = holding ? FUNC_READ_HOLDING_REGISTER : FUNC_READ_INPUT_REGISTERS;
    uint16_t pl_crc = crc(payload, payload_len); // this is already in little endian format
        payload[INDEX_READ_CRC_LSB] = (uint8_t)(pl_crc >> 8);
        payload[INDEX_READ_CRC_MSB] = (uint8_t)pl_crc;
        payload_len = PAYLOAD_HEADER_LEN + 2;
}

void ReadRegister::start_transfer() {
    modbus->start(payload, payload_len, rxbuf, READ_EXPECTED_CHARACTERS + 2*number_of_registers, this);
}

bool ReadRegister::is_ok() {
    if (crc(rxbuf, rx_len) != 0) return false;
    if (rx_len < READ_EXPECTED_CHARACTERS) return false; // error packet is 5 bytes
    return true;
}

WriteRegister::WriteRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address, uint8_t number_of_registers) :
MODBUSRegister(modbus, device_address, register_address, number_of_registers) {
    payload[INDEX_FUNC] = FUNC_WRITE_MULTIPLE_REGISTERS;
    // rest of the payload is constructed at start of writing
}

bool WriteRegister::is_ok() {
    if (crc(rxbuf, rx_len) != 0) return false;
    if (rx_len < WRITE_EXPECTED_CHARACTERS) return false; // error packet is 5 bytes
    return true;
}

void WriteRegister::start_transfer(uint16_t data) {
    payload[INDEX_DATA_BYTE_COUNT] = 2;
    payload[INDEX_DATA_MSB] = (uint8_t)(data >> 8);
    payload[INDEX_DATA_LSB] = (uint8_t)data;
    payload_len = PAYLOAD_HEADER_LEN + 3;
    uint pl_crc = crc(payload, payload_len);
    payload[INDEX_WRITE_CRC_LSB] = (uint8_t)(pl_crc >> 8);
    payload[INDEX_WRITE_CRC_MSB] = (uint8_t)pl_crc;
    payload_len += 2;

    modbus->start(payload, payload_len, rxbuf, WRITE_EXPECTED_CHARACTERS, this);
}

void WriteRegister::start_transfer(uint32_t data) {
    if (number_of_registers == 1) {
        start_transfer((uint16_t)data);
        return;
    }
    payload[INDEX_DATA_BYTE_COUNT] = 4; // 4*8=32
    payload_len = PAYLOAD_HEADER_LEN + 1;
    payload[payload_len++] = (uint8_t)(data >> 8); // big endian
    payload[payload_len++] = (uint8_t)data; // least significant word
    payload[payload_len++] = (uint8_t)(data >> 24); // big endian
    payload[payload_len++] = (uint8_t)(data >> 16); // most significant word
    uint pl_crc = crc(payload, payload_len);
    payload[payload_len++] = (uint8_t)(pl_crc >> 8);
    payload[payload_len++] = (uint8_t)pl_crc;

    modbus->start(payload, payload_len, rxbuf, WRITE_EXPECTED_CHARACTERS, this);
}


void WriteRegister::start_transfer(float data) {
    union { float f; uint32_t i; } d;
    d.f = data; // quick and dirty float bits to int cast
    start_transfer(d.i);
}

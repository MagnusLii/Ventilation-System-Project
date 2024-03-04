#include "pico/stdlib.h"

#include "modbus_register.h"

#define FUNC_READ_HOLDING_REGISTER 0x3
#define FUNC_WRITE_MULTIPLE_REGISTERS 0x10

#define INDEX_DEVADDR 0
#define INDEX_FUNC 1
#define INDEX_REGADDR_MSB 2
#define INDEX_REGADDR_LSB 3
#define INDEX_REG_COUNT_MSB 4
#define INDEX_REG_COUNT_LSB 5
#define INDEX_READ_CRC_LSB 6
#define INDEX_READ_CRC_MSB 7

#define INDEX_DATA_BYTE_COUNT 6
#define INDEX_DATA_LSB 7
#define INDEX_DATA_MSB 8
#define INDEX_WRITE_CRC_LSB 9
#define INDEX_WRITE_CRC_MSB 10

#define READ_EXPECTED_CHARACTERS 7
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

MODBUSRegister::MODBUSRegister(shared_modbus modbus, uint8_t device_address, uint16_t register_address, bool read) :
    modbus(modbus), read(read) {
    payload[INDEX_DEVADDR] = device_address;
    payload[INDEX_REGADDR_MSB] = (uint8_t)(register_address >> 8);
    payload[INDEX_REGADDR_LSB] = (uint8_t)register_address;
    payload[INDEX_REG_COUNT_MSB] = 0; // not doing that many registers at once
    payload[INDEX_REG_COUNT_LSB] = 1; // just doing one (for now)
    payload_len = 6;
    if (read) {
        payload[INDEX_FUNC] = FUNC_READ_HOLDING_REGISTER;
        uint16_t pl_crc = crc(payload, payload_len); // this is already in little endian format
        payload[INDEX_READ_CRC_LSB] = (uint8_t)(pl_crc >> 8);
        payload[INDEX_READ_CRC_MSB] = (uint8_t)pl_crc;
        payload_len = 8;
    }
}

void MODBUSRegister::start_transfer(uint16_t data) {
    if (!read) {
        payload[INDEX_DATA_LSB] = (uint8_t)data;
        payload[INDEX_DATA_MSB] = (uint8_t)(data >> 8);
        payload_len = 8;
        uint pl_crc = crc(payload, payload_len);
        payload[INDEX_READ_CRC_LSB] = (uint8_t)(pl_crc >> 8);
        payload[INDEX_READ_CRC_MSB] = (uint8_t)pl_crc;
        payload_len = 10;
    }
    modbus->start(payload, payload_len, rxbuf, (read) ? READ_EXPECTED_CHARACTERS : WRITE_EXPECTED_CHARACTERS);
}
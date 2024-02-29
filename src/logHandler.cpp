#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "eeprom.h"

#define CRC_LEN 2
#define LOG_LEN 6                     // Does not include CRC
#define LOG_ARR_LEN LOG_LEN + CRC_LEN // Includes CRC

#define DISPENSER_STATE_LEN 6                                 // Does not include CRC
#define DISPENSER_STATE_ARR_LEN DISPENSER_STATE_LEN + CRC_LEN // Includes CRC

#define REBOOT_STATUS_ADDR LOG_END_ADDR + LOG_SIZE

#define LOG_START_ADDR 0
#define LOG_END_ADDR 2048
#define LOG_SIZE 8
#define MAX_LOGS LOG_END_ADDR / LOG_SIZE

class LogHandler {
    public:
        void defineLogHandler(int logAddress, int rebootStatusAddress) {
            this->logAddress = logAddress;
            this->rebootStatusAddress = rebootStatusAddress;
        }
    
        void printPrivates() {
            std::cout << "Log Address: " << logAddress << std::endl;
            std::cout << "Reboot Status Address: " << rebootStatusAddress << std::endl;
        }

        uint16_t crc16(const uint8_t *data, size_t length) {
            uint8_t x;
            uint16_t crc = 0xFFFF;

            while (length--) {
                x = crc >> 8 ^ *data++;
                x ^= x >> 4;
                crc = (crc << 8) ^ (static_cast<uint16_t>(x << 12)) ^ (static_cast<uint16_t>(x << 5)) ^ static_cast<uint16_t>(x);
            }

            return crc;
        }

        int appendCrcToBase8Array(uint8_t *base8Array, int base8ArrayLen) {
            uint16_t crc = crc16(base8Array, base8ArrayLen - CRC_LEN);
            base8Array[base8ArrayLen - 2] = crc & 0xFF;
            base8Array[base8ArrayLen - 1] = (crc >> 8) & 0xFF;
            return base8ArrayLen + CRC_LEN;
        }

        


    private:
        int logAddress;
        int rebootStatusAddress;
};

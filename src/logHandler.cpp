#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include "eeprom.h"
#include "logHandler.h"

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

void appendCrcToBase8Array(uint8_t *base8Array, int *arrayLen){
    uint16_t crc = crc16(base8Array, *arrayLen);
    base8Array[*arrayLen] = crc >> 8;       // MSB
    base8Array[*arrayLen + 1] = crc & 0xFF; // LSB
    *arrayLen += CRC_LEN; // Update the array length to reflect the addition of the CRC
}

int getChecksum(uint8_t *base8Array, int base8ArrayLen) {
    uint16_t crc = crc16(base8Array, base8ArrayLen);
    return crc;
}

bool verifyDataIntegrity(uint8_t *base8Array, int base8ArrayLen) {
    if (getChecksum(base8Array, base8ArrayLen) == 0) {return true;}
    return false;
}

void enterLogToEeprom(uint8_t *base8Array, int *arrayLen, int logAddr) {
    uint8_t crcAppendedArray[*arrayLen + CRC_LEN];
    memcpy(crcAppendedArray, base8Array, *arrayLen); // copy original array to extended array
    appendCrcToBase8Array(crcAppendedArray, arrayLen); 
    for(int i = 0; i < *arrayLen; i++){
        printf("%d ", crcAppendedArray[i]);
    }
    std::cout << std::endl;
    eeprom_write_page(logAddr, crcAppendedArray, *arrayLen);
}

void zeroAllLogs(){
    int count = 0;
    uint16_t logAddr = 0;

    // Iterate through logs and mark them as not in use by setting the first byte to 0
    while (count < MAX_LOGS)
    {
        eeprom_write_byte(logAddr, 0);
        logAddr += LOG_SIZE;
        count++;
    }
}

int createLogArray(uint8_t *array, int messageCode, uint32_t timestamp){
    array[LOG_USE_STATUS] = 1;           // Mark log as in use
    array[MESSAGE_CODE] = messageCode;

    // TODO: redo replace timestamp with realtime date once RTC is implemented.
    array[TIMESTAMP_LSB] = (uint8_t)(timestamp & 0xFF);         // LSB of timestamp
    array[TIMESTAMP_MSB2] = (uint8_t)((timestamp >> 8) & 0xFF);
    array[TIMESTAMP_MSB1] = (uint8_t)((timestamp >> 16) & 0xFF);
    array[TIMESTAMP_MSB] = (uint8_t)((timestamp >> 24) & 0xFF); // MSB of timestamp

    return LOG_LEN; // Return the length of the filled log array (LOG_LEN)
}

int findFirstAvailableLog(){
    int count = 0;
    uint16_t logAddr = 0;

    // Find the first available log entry
    while (count < MAX_LOGS){
        if (eeprom_read_byte(logAddr) == 0){
            return count; // Return index if an available log entry is found.
        }
        logAddr += LOG_SIZE; // Move to the next log entry
        count++;
    }

    zeroAllLogs(); // Clear all logs if all entries are full
    return 0;      // Return 0 after clearing logs
}

// TODO: Remove once RTC is implemented.
uint32_t getTimestampSinceBoot(const uint64_t bootTimestamp){
    return (uint32_t)((time_us_64() - bootTimestamp) / 1000000);
}

// TODO: modify once RTC is implemented.
void printValidLogs(){
    for (int i = 0; i < MAX_LOGS; i++){
        uint16_t logAddr = i * LOG_SIZE; // Calculate the EEPROM address for the log entry
        uint8_t logData[LOG_ARR_LEN];

        eeprom_read_page(logAddr, logData, LOG_ARR_LEN);
        
        int tmp_log_array_length = LOG_ARR_LEN;
        if (logData[LOG_USE_STATUS] == 1 && verifyDataIntegrity(logData, tmp_log_array_length) == true){                                     
            // Check if the log entry is valid (non-zero message code)
            uint8_t messageCode = logData[MESSAGE_CODE]; // Extract the message code
            uint32_t timestamp = (logData[TIMESTAMP_MSB] << 24) | (logData[TIMESTAMP_MSB1] << 16) | (logData[TIMESTAMP_MSB2] << 8) | logData[TIMESTAMP_LSB];
            uint16_t timestamp_s = timestamp / 1000;
            // Construct timestamp from individual bytes

            printf("%d: %s %u seconds after last boot.\n", i, logMessages[messageCode], timestamp_s);
        }
    }
}

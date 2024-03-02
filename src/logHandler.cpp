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

#define LOG_START_ADDR 0
#define LOG_END_ADDR LOG_START_ADDR + (MAX_LOGS * LOG_SIZE)  // The final address is never actually used due the starting from 0. Acts as a separator for the next log type
#define LOG_SIZE 8
#define MAX_LOGS 256

#define REBOOT_STATUS_START_ADDR LOG_END_ADDR + LOG_SIZE
#define REBOOT_STATUS_END_ADDR REBOOT_STATUS_START_ADDR + (MAX_LOGS * LOG_SIZE)

// TODO: Create better messages
const char *logMessages[] = {
    "Test",
    "Test2"
};

// TODO: Create better messages
const char *rebootStatusMessages[] = {
    "OK",
    "Crash",
    "Forced Reboot"
};

// TODO: Delete this.
void LogHandler::printPrivates() {
    std::cout << "Log Address: " << this->unusedLogIndex << std::endl;
    std::cout << "Reboot Status Address: " << this->unusedRebootStatusIndex << std::endl;
}

void LogHandler::incrementUnusedLogIndex() {
    this->unusedLogIndex += 1;
}

void LogHandler::incrementUnusedRebootIndex() {
    this->unusedRebootStatusIndex += 1;
}

void LogHandler::pushLog(LogMessage messageCode){
    std::cout << "Pushing log: " << logMessages[messageCode] << " to address: " << (this->unusedLogIndex * LOG_SIZE) << std::endl;
    uint8_t logArray[LOG_LEN];
    int logLen = LOG_LEN;
    uint32_t timestamp = getTimestampSinceBoot(this->bootTimestamp);
    createLogArray(logArray, messageCode, timestamp);
    LogHandler::enterLogToEeprom(logArray, &logLen, (this->unusedLogIndex * LOG_SIZE));
    LogHandler::incrementUnusedLogIndex();

    // TODO: add mqtt message.
}

void LogHandler::pushRebootLog(RebootStatusCodes statusCode){
    std::cout << "Pushing reboot log: " << rebootStatusMessages[statusCode] << " to address: " << (this->unusedRebootStatusIndex * LOG_SIZE) << std::endl;
    uint8_t logArray[LOG_LEN];
    int logLen = LOG_LEN;
    uint32_t timestamp = getTimestampSinceBoot(this->bootTimestamp);
    createLogArray(logArray, statusCode, timestamp);
    LogHandler::enterLogToEeprom(logArray, &logLen, (this->unusedLogIndex * LOG_SIZE));
    LogHandler::incrementUnusedRebootIndex();
    
    // TODO: add mqtt message.
}

void LogHandler::zeroAllLogs(const LogType logType){
    uint16_t logAddr = 0;

    switch (logType){
    case LOGTYPE_MSG_LOG:
        logAddr = LOG_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            eeprom_write_byte(logAddr, 0);
            logAddr += LOG_SIZE;
        }
        this->unusedLogIndex = LOG_START_ADDR / LOG_SIZE;

        break;
    case LOGTYPE_REBOOT_STATUS:
        logAddr = REBOOT_STATUS_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            eeprom_write_byte(logAddr, 0);
            logAddr += LOG_SIZE;
        }
        this->unusedRebootStatusIndex = REBOOT_STATUS_START_ADDR / LOG_SIZE;

        break;
    }
}

// TODO: Replace switch with dynamic index placement, once other stuff is done.
void LogHandler::findFirstAvailableLog(const LogType logType){
    uint16_t logAddr = 0;

    switch (logType){
    case LOGTYPE_MSG_LOG:
        logAddr = LOG_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            if ((int)eeprom_read_byte(logAddr) == 0){
                this->unusedLogIndex = logAddr / LOG_SIZE;
                return;
            }
            logAddr += LOG_SIZE;
        }

        LogHandler::zeroAllLogs(LOGTYPE_MSG_LOG);
        this->unusedLogIndex = LOG_START_ADDR / LOG_SIZE;

        break;
    case LOGTYPE_REBOOT_STATUS:
        logAddr = REBOOT_STATUS_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            if ((int)eeprom_read_byte(logAddr) == 0){
                this->unusedRebootStatusIndex = logAddr / LOG_SIZE;
                return;
            }
            logAddr += LOG_SIZE;
        }

        LogHandler::zeroAllLogs(LOGTYPE_REBOOT_STATUS);
        this->unusedRebootStatusIndex = REBOOT_STATUS_START_ADDR / LOG_SIZE;

        break;
    }

    return;
}

void LogHandler::enterLogToEeprom(uint8_t *base8Array, int *arrayLen, const int logAddr) {
    uint8_t crcAppendedArray[*arrayLen + CRC_LEN];
    memcpy(crcAppendedArray, base8Array, *arrayLen); // copy original array to extended array
    appendCrcToBase8Array(crcAppendedArray, arrayLen);
    /*
    std::cout << "crcAppendedArray: ";
    for (int i = 0; i < *arrayLen; i++){
        std::cout << (int)crcAppendedArray[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "arrayLen: " << *arrayLen << std::endl;
    std::cout << "logAddr: " << logAddr << std::endl;
    */
    eeprom_write_page(logAddr, crcAppendedArray, *arrayLen);
}

void LogHandler::createLogArray(uint8_t *array, int messageCode, uint32_t timestamp){
    array[LOG_USE_STATUS] = LOG_IN_USE;
    array[MESSAGE_CODE] = messageCode;

    // TODO: redo replace timestamp with realtime date once RTC is implemented.
    array[TIMESTAMP_LSB] = (uint8_t)(timestamp & 0xFF);         // LSB of timestamp
    array[TIMESTAMP_MSB2] = (uint8_t)((timestamp >> 8) & 0xFF);
    array[TIMESTAMP_MSB1] = (uint8_t)((timestamp >> 16) & 0xFF);
    array[TIMESTAMP_MSB] = (uint8_t)((timestamp >> 24) & 0xFF); // MSB of timestamp

    return;
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

// TODO: Remove once RTC is implemented.
uint32_t getTimestampSinceBoot(const uint64_t bootTimestamp){
    return (uint32_t)((time_us_64() - bootTimestamp) / 1000000);
}

// TODO: modify once RTC is implemented.
void printValidLogs(LogType logType){
    uint16_t logAddr = 0;
    uint8_t logData[LOG_ARR_LEN];
    int tmp_log_array_length = LOG_ARR_LEN;

    switch (logType){
        case LOGTYPE_MSG_LOG:
            logAddr = LOG_START_ADDR;
            for (int i = 0; i < MAX_LOGS; i++){

            eeprom_read_page(logAddr, logData, LOG_ARR_LEN);
            /*
            int crc = getChecksum(logData, tmp_log_array_length);
            bool integrity = verifyDataIntegrity(logData, tmp_log_array_length);
            std::cout << std::endl << std::endl;
            std::cout << "Ind: " << i << " logData: ";
            for (int j = 0; j < tmp_log_array_length; j++){
                std::cout << (int)logData[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "crc: " << crc << std::endl;
            std::cout << "integrity: " << integrity << std::endl;
            */
            if (logData[LOG_USE_STATUS] == 1 && verifyDataIntegrity(logData, tmp_log_array_length) == true){
                uint8_t messageCode = logData[MESSAGE_CODE];
                uint32_t timestamp = (logData[TIMESTAMP_MSB] << 24) | (logData[TIMESTAMP_MSB1] << 16) | (logData[TIMESTAMP_MSB2] << 8) | logData[TIMESTAMP_LSB];
                uint16_t timestamp_s = timestamp / 1000;

                std::cout << logAddr << ": " << messageCode << " " << timestamp_s << std::endl;
                std::cout << logAddr << ": " << logMessages[messageCode] << " " << timestamp_s << " seconds after last boot." << std::endl;
            }
            logAddr += LOG_SIZE;
            }   
            break;

        case LOGTYPE_REBOOT_STATUS:
            logAddr = REBOOT_STATUS_START_ADDR;
            for (int i = 0; i < MAX_LOGS; i++){

            eeprom_read_page(logAddr, logData, LOG_ARR_LEN);
            /*
            int crc = getChecksum(logData, tmp_log_array_length);
            bool integrity = verifyDataIntegrity(logData, tmp_log_array_length);
            std::cout << std::endl << std::endl;
            std::cout << "Ind: " << i << " logData: ";
            for (int j = 0; j < tmp_log_array_length; j++){
                std::cout << (int)logData[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "crc: " << crc << std::endl;
            std::cout << "integrity: " << integrity << std::endl;
            */
            if (logData[LOG_USE_STATUS] == 1 && verifyDataIntegrity(logData, tmp_log_array_length) == true){
                uint8_t messageCode = logData[MESSAGE_CODE];
                uint32_t timestamp = (logData[TIMESTAMP_MSB] << 24) | (logData[TIMESTAMP_MSB1] << 16) | (logData[TIMESTAMP_MSB2] << 8) | logData[TIMESTAMP_LSB];
                uint16_t timestamp_s = timestamp / 1000;

                std::cout << logAddr << ": " << messageCode << " " << timestamp_s << std::endl;
                std::cout << logAddr << ": " << rebootStatusMessages[messageCode] << " " << timestamp_s << " seconds after last boot." << std::endl;
            }
            logAddr += LOG_SIZE;
            }   
            break;


}
}
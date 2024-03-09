#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include "eeprom.h"
#include "logHandler.h"
#include "commhandler.h"
#include "debugprint.h"
#include <memory.h>

#define CRC_LEN 2
#define LOG_LEN 6                     // Does not include CRC
#define LOG_ARR_LEN (LOG_LEN + CRC_LEN) // Includes CRC

#define DISPENSER_STATE_LEN 6                                 // Does not include CRC
#define DISPENSER_STATE_ARR_LEN (DISPENSER_STATE_LEN + CRC_LEN) // Includes CRC

#define LOG_START_ADDR 0
#define LOG_END_ADDR (LOG_START_ADDR + (MAX_LOGS * LOG_SIZE))  // The final address is never actually used due the starting from 0. Acts as a separator for the next log type
#define LOG_SIZE 8
#define MAX_LOGS 256

#define REBOOT_STATUS_START_ADDR (LOG_END_ADDR + LOG_SIZE)
#define REBOOT_STATUS_END_ADDR (REBOOT_STATUS_START_ADDR + (MAX_LOGS * LOG_SIZE))

#define CREDENTIALS_ARR_SIZE 64
#define CREDENTIALS_STORAGE_BLOCK 256 
#define CREDENTIALS_START_ADDR (REBOOT_STATUS_END_ADDR + (CREDENTIALS_ARR_SIZE - (REBOOT_STATUS_END_ADDR % CREDENTIALS_ARR_SIZE))) // Aligns the address to the next 64 byte boundary.
#define CREDENTIALS_NUM 4
#define CREDENTIALS_END_ADDR (CREDENTIALS_START_ADDR + (CREDENTIALS_STORAGE_BLOCK * CREDENTIALS_NUM))

// TODO: Create better messages
const char *logMessages[] = {
    "Test",
    "Test2",
};

// TODO: Create better messages
const char *rebootStatusMessages[] = {
    "OK",
    "Crash",
    "Forced Reboot"
};

// TODO: Delete this.
void LogHandler::printPrivates() {
    std::cout << "Log Address: " << this->unusedLogAddr << std::endl;
    std::cout << "Reboot Status Address: " << this->unusedRebootStatusAddr << std::endl;
    std::cout << "Unused Comm Config Address: " << this->unusedCommConfigAddr << std::endl;
    std::cout << "Current Comm Config Address: " << this->currentCommConfigAddr << std::endl;
}

void LogHandler::incrementUnusedLogIndex(const LogType logType) {
    if (logType == LOGTYPE_MSG_LOG){
        this->unusedLogAddr += LOG_SIZE;
        if (this->unusedLogAddr >= LOG_END_ADDR){
            this->clearAllLogs(LOGTYPE_MSG_LOG);
        }
    }
    else if (logType == LOGTYPE_REBOOT_STATUS){
        this->unusedRebootStatusAddr += LOG_SIZE;
        if (this->unusedRebootStatusAddr >= REBOOT_STATUS_END_ADDR){
            this->clearAllLogs(LOGTYPE_REBOOT_STATUS);
        }
    }
    else if (logType == LOGTYPE_COMM_CONFIG){
        eeprom_write_byte(this->currentCommConfigAddr, 0);
        this->currentCommConfigAddr = unusedCommConfigAddr;
        this->unusedCommConfigAddr += CREDENTIALS_STORAGE_BLOCK;
        if (this->unusedCommConfigAddr >= CREDENTIALS_END_ADDR){
            this->unusedCommConfigAddr = CREDENTIALS_START_ADDR;
        }
    }
}

void LogHandler::pushLog(LogMessage messageCode){
    uint8_t logArray[LOG_LEN];
    int logLen = LOG_LEN;
    uint32_t timestamp = getTimestampSinceBoot(this->bootTimestamp);
    createLogArray(logArray, messageCode, timestamp);
    LogHandler::enterLogToEeprom(logArray, &logLen, this->unusedLogAddr);
    LogHandler::incrementUnusedLogIndex(LOGTYPE_MSG_LOG);

    // string editing.
    std::string log = logMessages[messageCode];
    std::string message = "\"{\"log\":\"" + log + "\"}\"";
    DPRINT(message);
    
    //this->commHandler->publish(TopicType::LOG_SEND, message.c_str());
}

void LogHandler::pushRebootLog(RebootStatusCodes statusCode){
    uint8_t logArray[LOG_LEN];
    int logLen = LOG_LEN;
    uint32_t timestamp = getTimestampSinceBoot(this->bootTimestamp);
    createLogArray(logArray, statusCode, timestamp);
    LogHandler::enterLogToEeprom(logArray, &logLen, this->unusedRebootStatusAddr);
    LogHandler::incrementUnusedLogIndex(LOGTYPE_REBOOT_STATUS);
    
    // string editing.
    std::string log = rebootStatusMessages[statusCode];
    std::string message = "\"{\"devStatus\":\"" + log + "\"}\"";
    DPRINT(message);
    
    //this->commHandler->publish(TopicType::STATUS_SEND, message.c_str());

}

void LogHandler::clearAllLogs(const LogType logType){
    uint16_t logAddr = 0;

    if (logType == LOGTYPE_MSG_LOG){
        logAddr = LOG_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            eeprom_write_byte(logAddr, 0);
            logAddr += LOG_SIZE;
        }
        this->unusedLogAddr = LOG_START_ADDR;
    }
    else if (logType == LOGTYPE_REBOOT_STATUS){
        logAddr = REBOOT_STATUS_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            eeprom_write_byte(logAddr, 0);
            logAddr += LOG_SIZE;
        }
        this->unusedRebootStatusAddr = REBOOT_STATUS_START_ADDR;
    }
    return;
}

void LogHandler::findFirstAvailableLog(const LogType logType){
    uint16_t logAddr = 0;

    switch (logType){
    case LOGTYPE_MSG_LOG:
        logAddr = LOG_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            if ((int)eeprom_read_byte(logAddr) == 0){
                this->unusedLogAddr = logAddr;
                return;
            }
            logAddr += LOG_SIZE;
        }

        LogHandler::clearAllLogs(LOGTYPE_MSG_LOG);
        this->unusedLogAddr = LOG_START_ADDR;

        break;
    case LOGTYPE_REBOOT_STATUS:
        logAddr = REBOOT_STATUS_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
            if ((int)eeprom_read_byte(logAddr) == 0){
                this->unusedRebootStatusAddr = logAddr;
                return;
            }
            logAddr += LOG_SIZE;
        }

        LogHandler::clearAllLogs(LOGTYPE_REBOOT_STATUS);
        this->unusedRebootStatusAddr = REBOOT_STATUS_START_ADDR;

        break;

    //TODO: verify this
    case LOGTYPE_COMM_CONFIG:
        logAddr = CREDENTIALS_START_ADDR;
        for (int i = 0; i < CREDENTIALS_NUM; i++){
            if ((int)eeprom_read_byte(logAddr) == 0){
                this->unusedCommConfigAddr = logAddr;
                this->currentCommConfigAddr = logAddr - CREDENTIALS_STORAGE_BLOCK;

                if (logAddr == CREDENTIALS_START_ADDR){ // if unusedCommConfigAddr == CREDENTIALS_START_ADDR, then the currentCommConfigAddr is CREDENTIALS_END_ADDR.
                this->currentCommConfigAddr = CREDENTIALS_END_ADDR;
                }
                return;
            }
            logAddr += CREDENTIALS_ARR_SIZE;
        }
        this->unusedCommConfigAddr = CREDENTIALS_START_ADDR;
        this->currentCommConfigAddr = CREDENTIALS_END_ADDR;
        break;
    }
    return;
}

void LogHandler::enterLogToEeprom(uint8_t *base8Array, int *arrayLen, const int logAddr) {
    uint8_t crcAppendedArray[*arrayLen + CRC_LEN];
    memcpy(crcAppendedArray, base8Array, *arrayLen);
    appendCrcToBase8Array(crcAppendedArray, arrayLen);
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
    // TODO: REMOVE TROUBLESHOOT CODE
    std::cout << "CRC: " << crc << std::endl;
    return crc;
}

bool verifyDataIntegrity(uint8_t *base8Array, int base8ArrayLen) {
    // TODO: REMOVE TROUBLESHOOT CODE
    for (int i = 0; i < base8ArrayLen; i++){
        std::cout << (int)base8Array[i] << " ";
    }
    std::cout << std::endl;

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

    if (logType == LOGTYPE_MSG_LOG){
        logAddr = LOG_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
        eeprom_read_page(logAddr, logData, LOG_ARR_LEN);
        if (logData[LOG_USE_STATUS] == 1 && verifyDataIntegrity(logData, tmp_log_array_length) == true){
            uint8_t messageCode = logData[MESSAGE_CODE];
            uint32_t timestamp = (logData[TIMESTAMP_MSB] << 24) | (logData[TIMESTAMP_MSB1] << 16) | (logData[TIMESTAMP_MSB2] << 8) | logData[TIMESTAMP_LSB];

            std::cout << logAddr << ": " << logMessages[messageCode] << " " << timestamp << " seconds after last boot." << std::endl;
        }
        logAddr += LOG_SIZE;
        }   
    }
    else if (logType == LOGTYPE_REBOOT_STATUS){
        logAddr = REBOOT_STATUS_START_ADDR;
        for (int i = 0; i < MAX_LOGS; i++){
        eeprom_read_page(logAddr, logData, LOG_ARR_LEN);
        if (logData[LOG_USE_STATUS] == 1 && verifyDataIntegrity(logData, tmp_log_array_length) == true){
            uint8_t messageCode = logData[MESSAGE_CODE];
            uint32_t timestamp = (logData[TIMESTAMP_MSB] << 24) | (logData[TIMESTAMP_MSB1] << 16) | (logData[TIMESTAMP_MSB2] << 8) | logData[TIMESTAMP_LSB];

            std::cout << logAddr << ": " << rebootStatusMessages[messageCode] << " " << timestamp << " seconds after last boot." << std::endl;
        }
        logAddr += LOG_SIZE;
        }   
    }
    return;
}

void LogHandler::setCommHandler(std::shared_ptr<CommHandler> commHandler){
    this->commHandler = commHandler;
    return;
}

int createCredentialArray(std::string str, uint8_t *arr){
    int length = str.length();
    if (length > 60){
        DPRINT("String too long.");
        // TODO: do something else to indicate that the string is too long.
        return -1;
    }
    arr[0] =  LOG_IN_USE;
    arr[1] = length + 5; // Length of the string + 2 bytes for the CRC + 2 bytes for the length.
    memcpy(arr + 2, str.c_str(), length + 1);
    return length + 3;
}

void LogHandler::storeCredentials(std::string ssid, std::string password, std::string hostname, std::string port){
    if (ssid.length() > 60 || password.length() > 60 || hostname.length() > 60 || port.length() > 60){
        DPRINT("Credentials too long.");
        // TODO: do something else to indicate that the credentials are too long.
        return;
    }

    uint8_t ssidArr[CREDENTIALS_ARR_SIZE];
    uint8_t passwordArr[CREDENTIALS_ARR_SIZE];
    uint8_t hostnameArr[CREDENTIALS_ARR_SIZE];
    uint8_t portArr[CREDENTIALS_ARR_SIZE];

    int ssidLen = createCredentialArray(ssid, ssidArr);
    int pwLen = createCredentialArray(password, passwordArr);
    int hostnameLen = createCredentialArray(hostname, hostnameArr);
    int portLen = createCredentialArray(port, portArr);

    // TODO: REMOVE TROUBLESHOOT CODE
    std::cout << "SSID CRC: " << std::endl;
    appendCrcToBase8Array(ssidArr, &ssidLen);
    for (int i = 0; i < ssidLen; i++){
        std::cout << (int)ssidArr[i] << " ";
    }
    std::cout << std::endl;
    appendCrcToBase8Array(passwordArr, &pwLen);
    appendCrcToBase8Array(hostnameArr, &hostnameLen);
    appendCrcToBase8Array(portArr, &portLen);

    eeprom_write_page(this->unusedCommConfigAddr + (CREDENTIALS_ARR_SIZE * SSID), ssidArr, ssidLen);
    eeprom_write_page(this->unusedCommConfigAddr + (CREDENTIALS_ARR_SIZE * PASSWORD), passwordArr, pwLen);
    eeprom_write_page(this->unusedCommConfigAddr + (CREDENTIALS_ARR_SIZE * HOSTNAME), hostnameArr, hostnameLen);
    eeprom_write_page(this->unusedCommConfigAddr + (CREDENTIALS_ARR_SIZE * PORT), portArr, portLen);

    LogHandler::incrementUnusedLogIndex(LOGTYPE_COMM_CONFIG);

    return;
}

void LogHandler::fetchCredentials(std::string *ssid, std::string *password, std::string *hostname, std::string *port, int (&arr)[4]){
    uint8_t ssidArr[CREDENTIALS_ARR_SIZE];
    uint8_t passwordArr[CREDENTIALS_ARR_SIZE];
    uint8_t hostnameArr[CREDENTIALS_ARR_SIZE];
    uint8_t portArr[CREDENTIALS_ARR_SIZE];
    
    // Default all values to 0.
    for (int i = 0; i < 4; i++){
        arr[i] = 0;
    }

    eeprom_read_page(this->currentCommConfigAddr, ssidArr, CREDENTIALS_ARR_SIZE);
    eeprom_read_page(this->currentCommConfigAddr, passwordArr, CREDENTIALS_ARR_SIZE);
    eeprom_read_page(this->currentCommConfigAddr, hostnameArr, CREDENTIALS_ARR_SIZE);
    eeprom_read_page(this->currentCommConfigAddr, portArr, CREDENTIALS_ARR_SIZE);

    // TODO: figure out what to proceed if the data is not valid.
    // TODO: Verify that the CRC is not included in the string.
    if (verifyDataIntegrity(ssidArr, (int)ssidArr[1]) == true){
        *ssid = std::string((char *)ssidArr);
        arr[0] = 1;
    }
    if (verifyDataIntegrity(passwordArr, (int)passwordArr[1]) == true){
        *password = std::string((char *)passwordArr);
        arr[1] = 1;
    }
    if (verifyDataIntegrity(hostnameArr, (int)hostnameArr[1]) == true){
        *hostname = std::string((char *)hostnameArr);
        arr[2] = 1;
    }
    if (verifyDataIntegrity(portArr, (int)portArr[1]) == true){
        *port = std::string((char *)portArr);
        arr[3] = 1;
    }
    return;
}

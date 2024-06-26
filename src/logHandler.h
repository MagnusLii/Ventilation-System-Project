#ifndef logHandler_h
#define logHandler_h

#include <stdint.h>
#include "pico/time.h"
#include <memory>
#include <stdlib.h>
#include "commhandler.h"

extern const char *logMessages[];
extern const char *rebootStatusMessages[];

typedef enum {
    TEST,
    TEST2
} LogMessage;

// TODO: rework once RTC is implemented
typedef enum {
    LOG_USE_STATUS,
    MESSAGE_CODE,
    TIMESTAMP_MSB,
    TIMESTAMP_MSB1,
    TIMESTAMP_MSB2,
    TIMESTAMP_LSB
} LogArray;

typedef enum {
    COMMCONFIG_LOG_USE_STATUS,
    COMMCONFIG_LEN
} CommConfigArray;

typedef enum {
    OK,
    CRASH,
    FORCED_REBOOT
} RebootStatusCodes;

typedef enum {
    LOGTYPE_MSG_LOG,
    LOGTYPE_REBOOT_STATUS,
    LOGTYPE_COMM_CONFIG
} LogType;

typedef enum {
    LOG_NOT_IN_USE,
    LOG_IN_USE
} LogUseStatus;

typedef enum {
    SSID,
    PASSWORD,
    HOSTNAME,
    PORT
} CommConfig;

class LogHandler {
    public:
        // TODO: modify timestammp once RTC is implemented.
        LogHandler() {
            // TODO: Calling member functions in constructor like this is kinda sketchy, rework maybe.
            LogHandler::findFirstAvailableLog(LOGTYPE_MSG_LOG);
            LogHandler::findFirstAvailableLog(LOGTYPE_REBOOT_STATUS);
            LogHandler::findFirstAvailableLog(LOGTYPE_COMM_CONFIG);
            bootTimestamp = (to_ms_since_boot(get_absolute_time()) / 1000);
        };
        
        void printPrivates();
        void incrementUnusedLogIndex(const LogType logType);
        void pushLog(LogMessage messageCode);
        void pushRebootLog(RebootStatusCodes statusCode);
        void clearAllLogs(const LogType logType);
        void findFirstAvailableLog(const LogType logType);
        void enterLogToEeprom(uint8_t *base8Array, int *arrayLen, const int logAddr);
        void createLogArray(uint8_t *array, int messageCode, uint32_t timestamp);
        void setCommHandler(std::shared_ptr<CommHandler> commHandler);
        void storeCredentials(const char* ssid, const char* password, const char* hostname, const int port);
        void fetchCredentials(char *ssid, char *password, char *hostname, int *port, int *arr);

    private:
        std::shared_ptr<CommHandler> commHandler;
        int unusedLogAddr;
        int unusedRebootStatusAddr;
        uint32_t bootTimestamp;
        int unusedCommConfigAddr;
        int currentCommConfigAddr;
};

uint16_t crc16(const uint8_t *data, size_t length);
void appendCrcToBase8Array(uint8_t *base8Array, int *arrayLen);
int getChecksum(uint8_t *base8Array, int base8ArrayLen);
bool verifyDataIntegrity(uint8_t *base8Array, int base8ArrayLen);
uint32_t getTimestampSinceBoot(const uint64_t bootTimestamp);
void printValidLogs(LogType logType);
int createCredentialArray(std::string str, uint8_t *arr);

#endif

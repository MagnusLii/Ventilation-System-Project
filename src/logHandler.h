#ifndef logHandler_h
#define logHandler_h

#include <stdint.h>
#include "pico/time.h"

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
    OK,
    CRASH,
    FORCED_REBOOT
} RebootStatusCodes;

typedef enum {
    LOGTYPE_MSG_LOG,
    LOGTYPE_REBOOT_STATUS
} LogType;

typedef enum {
    LOG_NOT_IN_USE,
    LOG_IN_USE
} LogUseStatus;

class LogHandler {
    public:
        // TODO: modify timestammp once RTC is implemented.
        LogHandler() {
            // TODO: Calling member functions in constructor like this is kinda sketchy, rework maybe.
            LogHandler::findFirstAvailableLog(LOGTYPE_MSG_LOG);
            LogHandler::findFirstAvailableLog(LOGTYPE_REBOOT_STATUS);
            bootTimestamp = (to_ms_since_boot(get_absolute_time()) / 1000);
        };
        
        void printPrivates();
        void incrementUnusedLogIndex();
        void incrementUnusedRebootIndex();
        void pushLog(LogMessage messageCode);
        void pushRebootLog(RebootStatusCodes statusCode);
        void zeroAllLogs(const LogType logType);
        void findFirstAvailableLog(const LogType logType);
        void enterLogToEeprom(uint8_t *base8Array, int *arrayLen, const int logAddr);
        void createLogArray(uint8_t *array, int messageCode, uint32_t timestamp);

    private:
        int unusedLogIndex;
        int unusedRebootStatusIndex;
        uint32_t bootTimestamp;
};

uint16_t crc16(const uint8_t *data, size_t length);
void appendCrcToBase8Array(uint8_t *base8Array, int *arrayLen);
int getChecksum(uint8_t *base8Array, int base8ArrayLen);
bool verifyDataIntegrity(uint8_t *base8Array, int base8ArrayLen);
uint32_t getTimestampSinceBoot(const uint64_t bootTimestamp);
void printValidLogs();

#endif

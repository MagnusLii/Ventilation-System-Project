#ifndef logHandler_h
#define logHandler_h

#include <stdint.h>

extern const char *logMessages[];
extern const char *pillDispenserStatus[];

typedef enum {
    IDLE,
    DISPENSING,
    FULL_CALIBRATION,
    HALF_CALIBRATION
} reboot_num;

typedef enum {
    LOG_IDLE,
    LOG_WATCHDOG_REBOOT,
    LOG_DISPENSE1,
    LOG_DISPENSE2,
    LOG_DISPENSE3,
    LOG_DISPENSE4,
    LOG_DISPENSE5,
    LOG_DISPENSE6,
    LOG_DISPENSE7,
    LOG_HALF_CALIBRATION,
    LOG_FULL_CALIBRATION,    
    LOG_BUTTON_PRESS,
    LOG_PILL_DISPENSED,
    LOG_PILL_ERROR,
    LOG_DISPENSER_EMPTY,
    LOG_CALIBRATION_FINISHED,
    LOG_DISPENSE1_ERROR,
    LOG_DISPENSE2_ERROR,
    LOG_DISPENSE3_ERROR,
    LOG_DISPENSE4_ERROR,
    LOG_DISPENSE5_ERROR,
    LOG_DISPENSE6_ERROR,
    LOG_DISPENSE7_ERROR,
    LOG_HALF_CALIBRATION_ERROR,
    LOG_FULL_CALIBRATION_ERROR,
    LOG_GREMLINS,
    LOG_DISPENSER_STATUS_READ_ERROR,
    LOG_BOOTFINISHED,
    NOSEND
} log_number;

typedef enum {
    LOG_USE_STATUS,
    MESSAGE_CODE,
    TIMESTAMP_MSB,
    TIMESTAMP_MSB1,
    TIMESTAMP_MSB2,
    TIMESTAMP_LSB
} LogArray;

class LogHandler {
    public:
        // TODO: modify to find log entries automatically later.
        LogHandler(int unusedLogIndex, int unusedRebootStatusIndex) {
            this->unusedLogIndex = unusedLogIndex;
            this->unusedRebootStatusIndex = unusedRebootStatusIndex;
        }
        
        void printPrivates() {
            std::cout << "Log Address: " << unusedLogIndex << std::endl;
            std::cout << "Reboot Status Address: " << unusedRebootStatusIndex << std::endl;
        }

        static void pushLogToEeprom(LogHandler *logHandlerObject, log_number messageCode, uint32_t bootTimestamp){}
        static void updateUnusedLogIndex(LogHandler *logHandlerObject){}

    private:
        int unusedLogIndex;
        int unusedRebootStatusIndex;
};

uint16_t crc16(const uint8_t *data, size_t length);
void appendCrcToBase8Array(uint8_t *base8Array, int *arrayLen);
int getChecksum(uint8_t *base8Array, int base8ArrayLen);
bool verifyDataIntegrity(uint8_t *base8Array, int base8ArrayLen);
void enterLogToEeprom(uint8_t *base8Array, int *arrayLen, int logAddr);
void zeroAllLogs();
int createLogArray(uint8_t *array, int messageCode, uint32_t timestamp);
int findFirstAvailableLog();
uint32_t getTimestampSinceBoot(const uint64_t bootTimestamp);
void pushLogToEeprom(LogHandler *logHandlerObject, log_number messageCode, uint32_t bootTimestamp);
void updateUnusedLogIndex(LogHandler *logHandlerObject);
void printValidLogs();

#endif

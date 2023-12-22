#ifndef CAMBADA_RTDB2ERRORS_H
#define CAMBADA_RTDB2ERRORS_H

#include <string>

enum RtDB2ErrorCode {
    RTDB2_SUCCESS = 0,
    RTDB2_KEY_NOT_FOUND = -1,
    RTDB2_FAILED_PARSING_CONFIG_FILE = -2,
    RTDB2_REMOTE_NOT_FOUND = -3,
    RTDB2_INTEGER_ID_NOT_FOUND = -4,
    RTDB2_VALUE_POINTING_TO_NULL = -5,
    RTDB2_STORAGE_DOES_NOT_EXISTS = -6,
    RTDB2_FAILED_DECOMPRESSING = -7,
    RTDB2_FAILED_COMPRESSING = -8,
    RTDB2_FAILED_DESERIALIZE = -9,
    RTDB2_FAILED_SEMAPHORE_CREATION = -10,
    RTDB2_FAILED_SEMAPHORE_WAIT = -11,
    RTDB2_INTERNAL_MDB_ERROR = -12,
    RTDB2_ITEM_STALE = -13,
    RTDB2_FAILED_SEMAPHORE_RELEASE = -14
};

#define RTDB_PRINT_ERROR_CODE(error, message) \
    RtDB2_print_error_code(__FILE__, __FUNCTION__, error, message)
void RtDB2_print_error_code(const std::string& file, const std::string& funct,
        const int& error, const std::string& message);

#endif //CAMBADA_RTDB2ERRORS_H

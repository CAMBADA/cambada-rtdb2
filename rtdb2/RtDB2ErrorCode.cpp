#include <sstream>

#include "RtDB2ErrorCode.h"
#include "RtDB2Definitions.h"

static const int RtDB2_codes_as_warning[] = {
        RTDB2_SUCCESS,
        RTDB2_KEY_NOT_FOUND,
};

std::string RtDB2_error_tostring(const int& error) {
    switch (error) {
        case RTDB2_SUCCESS: return "Operation finished successfully";
        case RTDB2_KEY_NOT_FOUND: return "Key could not be found in RtDB2 storage";
        case RTDB2_FAILED_PARSING_CONFIG_FILE: return "Failed to parse rtdb2_configuration_xml";
        case RTDB2_REMOTE_NOT_FOUND: return "Remote storage could not be found";
        case RTDB2_INTEGER_ID_NOT_FOUND: return "Integer ID was not found in the configuration. "
                    "Did you run xrtdb? Check rtdb2_configuration.xml for oid attributes";
        case RTDB2_VALUE_POINTING_TO_NULL: return "Value is pointing to NULL! Cannot insert NULL.";
        case RTDB2_STORAGE_DOES_NOT_EXISTS: return "RtDB2 storage does not exists or process does not have access";
        case RTDB2_FAILED_DECOMPRESSING: return "Error while decompressing data! Dictionary might mismatch.";
        case RTDB2_FAILED_COMPRESSING: return "Error while compressing data! Dictionary might mismatch.";
        case RTDB2_FAILED_DESERIALIZE: return "Failed to deserialize data! Structure has several changes.";
        case RTDB2_FAILED_SEMAPHORE_CREATION: return "Failed to create semaphore on wait for PUT.";
        case RTDB2_FAILED_SEMAPHORE_WAIT: return "Failed waiting for semaphore";
        case RTDB2_INTERNAL_MDB_ERROR: return "Internal MDB error";
        case RTDB2_ITEM_STALE: return "Item stale";
        case RTDB2_FAILED_SEMAPHORE_RELEASE: return "Failed to release semaphore on PUT";
        default: return "Unknown error";
    }
}

void RtDB2_print_error_code(const std::string& file, const std::string& funct, 
        const int& error, const std::string& message) {
    bool found = false;
    for (unsigned int i = 0; i < sizeof(RtDB2_codes_as_warning) / sizeof(int); i++) {
        if (RtDB2_codes_as_warning[i] == error) {
            found = true;
            break;
        }
    }

    const char* message_c = message.c_str();
    const char* error_c = RtDB2_error_tostring(error).c_str();
    if (found) {
        RTDB_WARNING_FC(file.c_str(), funct.c_str(), "%s %s", message_c, error_c);
    } else {
        RTDB_ERROR_FC(file.c_str(), funct.c_str(), "%s %s", message_c, error_c);
    }
}

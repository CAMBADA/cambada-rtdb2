#ifndef CAMBADA_RTDBDEFINITIONS_H
#define CAMBADA_RTDBDEFINITIONS_H

#include <msgpack.hpp>

// Activate this flag to make every print visible, even debug ones.
//#define RTDB2_ACTIVE_DEBUG

// TODO cleanup this header, much of it seems private - the important part for clients are the SERIALIZE macro/aliases, so maybe rename this header to "serialize.hpp"?

#define SERIALIZE_DATA(...)         MSGPACK_DEFINE_MAP(__VA_ARGS__)
#define SERIALIZE_DATA_FIXED(...)   MSGPACK_DEFINE(__VA_ARGS__)
#define SERIALIZE_ENUM(enum_name)   MSGPACK_ADD_ENUM(enum_name)
#define RTDB2_DEFAULT_PATH          "/tmp/rtdb2_storage"
#define DB_PREPEND_NAME             "agent"
#define RTDB2_CONFIGURATION_FILE    "config/rtdb2_configuration.xml"
#define ZSTD2_DICTIONARY_FILE       "config/zstd_dictionary.dic"

// Default defines that are possible to call
// _FC means that it is possible to define the file and function caller
#define RTDB_ERROR(txt, par...) \
    RTDB_ERROR_FC(__FILE__, __FUNCTION__, txt, ##par)
#define RTDB_ERROR_FC(file, funct, txt, par...) \
    RTDB_PRINT(file, funct, txt, "ERROR", ##par)

#define RTDB_WARNING(txt, par...) \
    RTDB_WARNING_FC(__FILE__, __FUNCTION__, txt, ##par)
#define RTDB_WARNING_FC(file, funct, txt, par...) \
    RTDB_OPTIONAL(file, funct, txt, "WARNING", ##par)

#define RTDB_DEBUG(txt, par...) \
    RTDB_DEBUG_FC(__FILE__, __FUNCTION__, txt, ##par)
#define RTDB_DEBUG_FC(file, funct, txt, par...) \
    RTDB_OPTIONAL(file, funct, txt, "DEBUG", ##par)

// Baseline print (Should never be used directly unless
// It is a printed that is always required.
#define RTDB_PRINT(file, funct, txt, type, par...) \
        printf("[RtDB2] %s: (%s / %s): " txt "\n", type, file, funct, ##par)
// Logic related with optional prints
#ifdef RTDB2_ACTIVE_DEBUG
    #define RTDB_OPTIONAL(file, funct, txt, type, par...) \
        RTDB_PRINT(file, funct, txt, type, ##par)
#else
    #define RTDB_OPTIONAL(file, funct, txt, type, par...)
#endif

#endif //CAMBADA_RTDBDEFINITIONS_H

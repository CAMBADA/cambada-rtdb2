#ifndef RTDB2_ITEM_EXAMPLE_H
#define RTDB2_ITEM_EXAMPLE_H

#include <vector>

enum EnumStatus
{
    EnumExample_NOK = 0,
    EnumExample_OK,
    EnumExample_UNKNOWN
};

struct ExampleItem
{
    unsigned int value;
    EnumStatus status;

    // Include these items in the serialization of this structure (usually you want all, but you can skip some if needed)
    // Serialized data will NOT include the variable names (FIXED) - structs must match to deserialize
    SERIALIZE_DATA_FIXED(value, status);
};

struct ExampleItemMapped
{
    int val1;
    std::vector<int> array;

    // Include these items in the serialization of this structure
    // Serialized data WILL include the variable names
    SERIALIZE_DATA(val1, array);
};


// This resolves enum names
SERIALIZE_ENUM(EnumStatus);

#endif


#ifndef _INCLUDED_RTDB2ITEM_H_
#define _INCLUDED_RTDB2ITEM_H_

#include <string>
#include "rtime.hpp"
#include "RtDB2ErrorCode.h"
#include "RtDB2Definitions.h" // for serialization
#include "serializer/RtDB2Serializer.h"


struct RtDB2Item
{
    std::string data;
    rtime       timestamp;
    bool        shared = false;
    bool        list = false;
    
    SERIALIZE_DATA_FIXED(data, timestamp, shared, list);
    
    // interpret data
    template <typename T>
    T value()
    {
        T val;
        RtDB2Serializer::deserialize(data, val);
        return val;
    }
    
    // return age
    float age()
    {
        return float(double(rtime::now() - timestamp)); // explicit downcast from double
    }
    
};

struct RtDB2FrameItem : public RtDB2Item
{
    // as above, but extended with:
    std::string key;
    int         agent;

    SERIALIZE_DATA_FIXED(key, agent, data, timestamp, shared, list);
    
    // conversion constructor
    RtDB2FrameItem()
        : RtDB2Item(),
        key(""),
        agent(0)
    {
    }
    
    RtDB2FrameItem(RtDB2Item const &item)
        : RtDB2Item(item),
        key(""),
        agent(0)
    {
    }
};

#endif


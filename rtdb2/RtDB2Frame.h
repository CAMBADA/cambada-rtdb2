#ifndef _INCLUDED_RTDB2FRAME_H_
#define _INCLUDED_RTDB2FRAME_H_

// note: a Frame is unaware of compression, only serialization
// if configured, (de-)compression is done at RtDB2 level

#include <vector>
#include "RtDB2Item.h"


struct RtDB2FrameSelection
{
    bool local = false;
    bool shared = false;
    bool unique = false;
    std::vector<int> agents; // empty = all
    float maxAge = 1.0; // seconds
};

struct RtDB2Frame
{
    // public datamembers
    std::vector<RtDB2FrameItem> items;
    
    SERIALIZE_DATA_FIXED(items);
    
    std::string toString()
    {
        std::string s;
        int r = RtDB2Serializer::serialize(*this, s);
        if (r != 0)
        {
            throw std::runtime_error("failed to serialize frame");
        }
        return s;
    }

    void fromString(std::string const &s)
    {
        int r = RtDB2Serializer::deserialize(s, *this);
        if (r != 0)
        {
            throw std::runtime_error("failed to deserialize frame string");
        }
    }

};


#endif


/*
 * frameheader.hpp
 *
 *  Created on: 2018-12-09
 *      Author: Jan Feitsma
 */


#ifndef _INCLUDED_RTDB2_COMM2_FRAMEHEADER_HPP_
#define _INCLUDED_RTDB2_COMM2_FRAMEHEADER_HPP_

#include "rtime.hpp"

struct FrameHeader
{
    unsigned char agentId;
    int counter;
    rtime timestamp;
    SERIALIZE_DATA_FIXED(agentId, counter, timestamp);
};

#endif


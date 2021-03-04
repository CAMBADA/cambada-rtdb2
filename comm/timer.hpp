/*
 * timer.hpp
 *
 *  Created on: 2018-12-09
 *      Author: Jan Feitsma
 */


#ifndef _INCLUDED_RTDB2_COMM2_TIMER_HPP_
#define _INCLUDED_RTDB2_COMM2_TIMER_HPP_


#include "rtime.hpp"

class Timer
{
public:
    Timer(float duration);
    ~Timer();
    
    // store desired duration
    void setDuration(float duration);
    
    // sleep until timer expires
    void sleep();
    
private:
    rtime _t0;
    float _duration = 1.0;
};

#endif


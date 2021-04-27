/*
 * timer.cpp
 *
 *  Created on: 2018-12-09
 *      Author: Jan Feitsma
 */


#include "timer.hpp"
#include "tprintf.hpp"


Timer::Timer(float duration)
{
    _duration = duration;
    _t0 = rtime::now();
}

Timer::~Timer()
{
}

void Timer::setDuration(float duration)
{
    _duration = duration;
}

void Timer::sleep()
{
    rtime t = rtime::now();
    float elapsed = double(t) - double(_t0);
    if (elapsed < 0)
    {
        elapsed = 0; // be robust for CPU clock corrections
    }
    int usecToSleep = int(1e6 * (_duration - elapsed));
    if (usecToSleep >= 0)
    {
        usleep(usecToSleep);
    }
    else
    {
        tprintf("usecToSleep=%d elapsed=%.6f _duration=%.3f _t0=%s t=%s", usecToSleep, elapsed, _duration, _t0.toStr().c_str(), t.toStr().c_str());
    }
    // prepare for next - typically exactly one iteration, but in case previous one took too long, we align on next beat
    int steps = 0;
    while (_t0 < t)
    {
        steps += 1;
        _t0 = _t0 + _duration;
    }
    // warn if steps > 1
    if (steps > 1)
    {
        tprintf("WARNING: iteration took too long (elapsed=%.3fs, steps=%d)", elapsed, steps);
    }
}


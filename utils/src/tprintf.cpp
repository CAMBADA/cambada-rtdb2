/**
 * File: tprintf.cpp
 * Author: Jan Feitsma
 * Creation: January 2019
 *
 */

#include "tprintf.hpp"

#include <cstdio>
#include "rtime.hpp"


void _tprintf_start()
{
    std::string timestr = rtime::now().toStr();
    printf("%s ", timestr.c_str());
}

void _tprintf_end()
{
    printf("\n");
    fflush(stdout);
}



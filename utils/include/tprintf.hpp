/**
 * File: tprintf.hpp
 * Author: Jan Feitsma
 * Creation: January 2019
 *
 * Description: wrapper around printf, pre-pending timestamp and always flushing.
 * Having the timestamp is convenient to find when something interesting occured. 
 * Consider for instance a fragment of standard output of comm process without timestamps:
 *   transmitting 33.8KB/s (averaged over 1.0s)
 *   transmitting 34.5KB/s (averaged over 1.0s)
 *   transmitting 73.8KB/s (averaged over 1.0s)
 *   transmitting 33.8KB/s (averaged over 1.0s)
 *   transmitting 33.8KB/s (averaged over 1.0s)
 * When did the 73KB/s spike occur?
 *
 */

#ifndef _INCLUDED_TPRINTF_HPP_
#define _INCLUDED_TPRINTF_HPP_

#include <cstdarg>
#include <cstdio>
#include <memory>

// uncomment to suppress all output
//#define TPRINTF_DISABLED

void _tprintf(const char *fmt, ...);

// tprintf macro
#ifndef TPRINTF_DISABLED
    #define tprintf(fmt, ...) { _tprintf(fmt, ##__VA_ARGS__); }
#else
    #define tprintf(...) ((void)0)
#endif

#endif


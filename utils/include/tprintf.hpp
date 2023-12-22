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

// uncomment to suppress all output
//#define TPRINTF_DISABLED

// helpers
void _tprintf_start();
void _tprintf_end();

// tprintf macro
#ifndef TPRINTF_DISABLED
    #define tprintf(fmt, ...) { _tprintf_start(); printf(fmt, ##__VA_ARGS__); _tprintf_end(); }
#else
    #define tprintf(...) ((void)0)
#endif

#endif


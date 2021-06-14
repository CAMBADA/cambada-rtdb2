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

void _tprintf_wrap(char const *msg)
{
    _tprintf_start();
    printf("%s", msg);
    _tprintf_end();
}

void _tprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char *tmp = 0;
    int res = vasprintf(&tmp, fmt, ap);
    va_end(ap);

    if (res != -1)
    {
        _tprintf_wrap(tmp);
        free(tmp);
    } // else: vasprintf strangely enough failed; no cleanup nor reporting needed
}

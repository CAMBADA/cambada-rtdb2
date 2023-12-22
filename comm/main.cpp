/*
 * main.cpp
 *
 *  Created on: 2018-12-08
 *      Author: Jan Feitsma
 */


#include "comm.hpp"


int main(int argc, char **argv)
{
    Comm c;
    // configuration is managed via RtDB2Configuration XML parsing

    // provide options to overrule? for instance interface (wlo1, eth, ...)
    // TODO: make nicer interface (argparse, but lightweight?)
    if (argc > 1)
    {
        c.dbPath = argv[1];
    }

    c.run();

    return 0;
}


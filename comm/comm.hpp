/*
 * comm.hpp
 *
 *  Created on: 2018-12-08
 *      Author: Jan Feitsma
 */


#ifndef _INCLUDED_RTDB2_COMM_HPP_
#define _INCLUDED_RTDB2_COMM_HPP_

#include <boost/thread/thread.hpp>

#include "../../rtdb2/RtDB2Store.h"
#include "MulticastSocket.h"
#include "timer.hpp"
#include "statistics.hpp"
#include "frameheader.hpp"


#define COMM_BUFFER_SIZE 65536

class Comm
{
public:
    Comm();
    ~Comm();
    
    void run();

    // public properties, overridable until initialize() is called
    int                   agentId;
    std::string           dbPath = RTDB2_DEFAULT_PATH;
    CommunicationSettings settings;
    
private:
    bool                  _initialized = false;
    boost::thread         _receiverThread;
    boost::thread         _transmitterThread;
    boost::thread         _diagnosticsThread;
    MulticastSocket       _socket;
    Statistics            _statistics;
    RtDB2                *_rtdb = NULL;
    int                   _counter = 0;
    
    // singular operations
    void receive();
    void transmit();
    
    // loops / threads
    void receiver();
    void transmitter();
    void diagnostics();
    
    // miscellaneous
    void initialize();
    static void sigHandler(int sig);
    void printSettings();
    void removeHeader(std::string &buffer, FrameHeader &header);
    void makeHeader(FrameHeader &header, int counter);
    void insertHeader(std::string &buffer, FrameHeader const &header);
    
};

#endif


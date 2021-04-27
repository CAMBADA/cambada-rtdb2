/*
 * statistics.hpp
 *
 *  Created on: 2018-12-09
 *      Author: Jan Feitsma
 */


#ifndef _INCLUDED_RTDB2_COMM2_STATISTICS_HPP_
#define _INCLUDED_RTDB2_COMM2_STATISTICS_HPP_


#include "rtime.hpp"


// TODO: serialize these structs to also store in RTDB / .rdl?

#define TIME_DELTAS_BUFFER_SIZE 1000

class TransmitStatistics
{
private:
    int _counter = 0;
    int _totalBytes = 0;
    rtime _lastReportTime = rtime::now();
    int _lastReportBytes = 0;

public:
    void update(int numBytes);
    std::string report();
};


class ReceiveStatistics
{
private:
    int _counter = 0;
    int _totalBytes = 0;
    int _packetsLost = 0;
    rtime _lastReportTime = rtime::now();
    int _lastReceivedId = 0;
    int _lastReportBytes = 0;
    int _lastReportLoss = 0;
    std::deque<double> _timeDeltas;

public:
    void update(int numBytes, int packetId, double dt);
    std::string report();
};


struct Statistics
{
    TransmitStatistics transmit;
    std::map<int, ReceiveStatistics> receive;
    
    void display();
};

#endif


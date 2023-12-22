/*
 * statistics.cpp
 *
 *  Created on: 2019-02-24
 *      Author: Jan Feitsma
 */


#include "statistics.hpp"
#include "tprintf.hpp"
#include <boost/thread/thread.hpp>
#include <numeric>


boost::mutex g_mutex_rtdb;


void TransmitStatistics::update(int numBytes)
{
    boost::mutex::scoped_lock l(g_mutex_rtdb);
    _counter++;
    _totalBytes += numBytes;
}

std::string TransmitStatistics::report()
{
    boost::mutex::scoped_lock l(g_mutex_rtdb);
    // calculate data rate
    rtime t = rtime::now();
    float elapsed = double(t - _lastReportTime);
    float bandWidth = (_totalBytes - _lastReportBytes) / 1024.0 / elapsed;
    // reset for next calculation
    _lastReportTime = t;
    _lastReportBytes = _totalBytes;
    // report
    char buffer[256] = {0};
    sprintf(buffer, "%6.2fKB/s", bandWidth);
    return buffer;
}

void ReceiveStatistics::update(int numBytes, int packetId, double dt)
{
    boost::mutex::scoped_lock l(g_mutex_rtdb);
    _counter++;
    _totalBytes += numBytes;
    _packetsLost += (packetId - _lastReceivedId - 1);
    _lastReceivedId = packetId;
    _timeDeltas.push_front(dt);
}

std::string ReceiveStatistics::report()
{
    boost::mutex::scoped_lock l(g_mutex_rtdb);
    // calculate data rate
    rtime t = rtime::now();
    float elapsed = double(t - _lastReportTime);
    float bandWidth = (_totalBytes - _lastReportBytes) / 1024.0 / elapsed;
    int packetsLost = _packetsLost;
    if (_timeDeltas.size() > TIME_DELTAS_BUFFER_SIZE)
    {
        _timeDeltas.resize(TIME_DELTAS_BUFFER_SIZE);
    }
    double avgTimeDelta = 0.0;
    if (_timeDeltas.size())
    {
        avgTimeDelta = accumulate(_timeDeltas.begin(), _timeDeltas.end(), 0.0) / _timeDeltas.size(); 
    }
    // reset for next calculation
    _lastReportTime = t;
    _lastReportBytes = _totalBytes;
    _packetsLost = 0;
    // report
    std::string rpt;
    char buffer[256] = {0};
    sprintf(buffer, "%6.2fKB/s", bandWidth);
    rpt = buffer;
    if (packetsLost)
    {
        sprintf(buffer, "%3d", -packetsLost);
        rpt += buffer;
    }
    else
    {
        rpt += "   ";
    }
    sprintf(buffer, " %5.1fms", 1e3 * avgTimeDelta);
    rpt += buffer;
    return rpt;
}

void Statistics::display()
{
    std::string msg;
    msg = "transmit " + transmit.report();
    if (receive.size())
    {
        msg += " receive";
        for (auto it = receive.begin(); it != receive.end(); ++it)
        {
            msg += " [" + std::to_string(it->first) + ":" + it->second.report() + "]";
        }
    }
    tprintf("%s", msg.c_str());
}


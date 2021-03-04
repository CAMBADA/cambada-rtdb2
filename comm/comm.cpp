/*
 * comm.cpp
 *
 * The communication library is responsible for syncing RTDB instances over wifi.
 * Part of RTDB package.
 * Improvements w.r.t. original comm:
 *  - sparse sync scheme: only send data recently changed
 *  - make use of configured data periodicity
 *  - include network statistics
 *
 *  Created on: 2018-12-08 (as Comm, later named back to comm)
 *      Author: Jan Feitsma
 */


#include <signal.h>
#include "comm.hpp"
#include "tprintf.hpp"


#ifdef RTDB2_Comm_DEBUG_MODE
    #define cdebug(...) tprintf(__VA_ARGS__)
#else
    #define cdebug(...) do {} while(0)
#endif



// global flag to connect static signal handler with threads
bool globalShutdownFlag = false;


Comm::Comm()
{
    // retrieve agentId and settings
    agentId = 0;
    char *envc = NULL;
    if ((envc = getenv("AGENT")) != NULL) 
    {
        agentId = atoi(envc);
    }
    settings = RtDB2(agentId).getConfiguration().get_communication_settings();
    // setup signal handler for (somewhat) graceful shutdown
    if (signal(SIGINT, Comm::sigHandler) == SIG_ERR) 
    {
        throw std::runtime_error("Error while setting up signal handler");
    }
    _initialized = false;
    dbPath = RTDB2_DEFAULT_PATH;
    _rtdb = NULL;
}

void Comm::initialize()
{
    // setup socket
    int r = _socket.openSocket(settings.interface, settings.multiCastIP, settings.port, settings.loopback);
    printSettings();
    if (r != 0)
    {
        throw std::runtime_error("Failed to setup socket");
    }
    // setup RTDB
    _rtdb = RtDB2Store::getInstance().getRtDB2(agentId, dbPath);
    _initialized = true;
}

Comm::~Comm()
{
}

void Comm::printSettings()
{
    std::cout << "Comm settings:" << std::endl;
    std::cout << "        agent = " << agentId << std::endl;
    std::cout << " databasePath = " << dbPath << std::endl;
    std::cout << "  multiCastIP = " << settings.multiCastIP << std::endl;
    std::cout << "    interface = " << _socket.getInterface() << std::endl;
    std::cout << "      address = " << _socket.getIpAddress() << std::endl;
    std::cout << "    frequency = " << settings.frequency << std::endl;
    std::cout << "         port = " << settings.port << std::endl;
    std::cout << "  compression = " << settings.compression << std::endl;
    std::cout << "     loopback = " << settings.loopback << std::endl;
    std::cout << "         send = " << settings.send << std::endl;
}

void Comm::receive()
{
    // read out the port
    char buffer[COMM_BUFFER_SIZE] = {0};
    int received = 0;
    if ((received = _socket.receiveData(buffer, COMM_BUFFER_SIZE)) > 0)
    {
        rtime tNow = rtime::now();
        cdebug("receive wait");
        cdebug("receive busy");
        std::string bufferStr(buffer, received);
        // remove and process header
        FrameHeader header;
        removeHeader(bufferStr, header);
        // put data into RTDB
        cdebug("receive agent=%d", header.agentId);
        int r = _rtdb->putFrameString(bufferStr, true); 
        // boolean flag true just overrules the timestamps
        // this is a containment ignore WIFI latency and clock diffs, just overrule timestamps
        // TODO: measure (via frame header) and model clock offset and latency, correct timestamps such that age becomes really accurate
        if (r != RTDB2_SUCCESS)
        {
            tprintf("WARNING: putFrame failed (returned %d)", r);
        }
        else
        {
            // statistics
            double dt = tNow - header.timestamp;
            _statistics.receive[header.agentId].update(received, header.counter, dt);
        }
        cdebug("receive finish");
    }
    else
    {
        tprintf("WARNING: receive failed (returned %d)", received);
    }
}

void Comm::receiver()
{
    while (!globalShutdownFlag)
    {
        receive(); // blocking
    }
}

void Comm::transmit()
{
    cdebug("transmit wait");
    cdebug("transmit busy");
    // query RTDB, which should take care of periodicity
    RtDB2FrameSelection frameSelection;
    frameSelection.local = false;
    frameSelection.shared = true;
    frameSelection.agents.push_back(agentId);
    frameSelection.maxAge = 1e4;
    std::string buffer;
    _rtdb->getFrameString(buffer, frameSelection, _counter);
    // prepend header before sending
    FrameHeader header;
    makeHeader(header, _counter);
    insertHeader(buffer, header);
    int numBytes = buffer.size();
    if (numBytes >= COMM_BUFFER_SIZE)
    {
        tprintf("WARNING: dropping too large frame (%d > %d)", numBytes, COMM_BUFFER_SIZE);
        return;
    }
    char raw[COMM_BUFFER_SIZE];
    memcpy(raw, buffer.c_str(), numBytes);
    _socket.sendData(raw, numBytes);
    // statistics
    _statistics.transmit.update(numBytes);
    _counter++;
    cdebug("transmit finish");
}


float addJitter(float t, float j)
{
    return t * (1.0 - (2.0 * rand() / RAND_MAX - 1.0) * j);
}

void Comm::transmitter()
{
    // we could use the timer to achieve an almost-stable (i.e. non-drifting) loop
    // but we add some jitter to reduce packet loss in case multiple transmitters are in sync ...
    float jitter1 = 0.03; // TODO make configurable
    float jitter2 = 0.05;
    float sleepTime = addJitter(1.0 / settings.frequency, jitter1);
    Timer timer(sleepTime);
    while (!globalShutdownFlag)
    {
        transmit();
        timer.sleep();
        timer.setDuration(addJitter(sleepTime, jitter2));
    }
}

void Comm::diagnostics()
{
    float sleepTime = 1.0;
    Timer timer(sleepTime);
    while (!globalShutdownFlag)
    {
        _statistics.display();
        timer.sleep();
    }
}

void Comm::run()
{
    tprintf("initializing");
    if (!_initialized)
    {
        initialize();
    }
    
    // start receiver thread
    tprintf("starting receiver thread");
    _receiverThread = boost::thread(&Comm::receiver, this);
    
    // start transmitter thread at given frequency
    if (settings.send)
    {
        tprintf("starting transmitter thread");
        _transmitterThread = boost::thread(&Comm::transmitter, this);
    }
    else
    {
        tprintf("skipping transmitter thread, we run in listen-only mode");
    }
    
    // start diagnostics thread, to report to stdout
    tprintf("starting diagnostics thread");
    _diagnosticsThread = boost::thread(&Comm::diagnostics, this);
    
    // join all
    _receiverThread.join();
    if (settings.send)
    {
        _transmitterThread.join();
    }
    _diagnosticsThread.join();
}

void Comm::sigHandler(int sig) 
{
    tprintf("received signal %d", sig);
    if ((sig == SIGINT) && (!globalShutdownFlag))
    {
        globalShutdownFlag = true;
    }
    else
    {
        // either repeating the interrupt signal, or we receive some other signal ... 
        abort();
    }
}

void Comm::removeHeader(std::string &buffer, FrameHeader &header)
{
    if (buffer.size() == 0)
    {
        tprintf("ERROR: Unexpected small packet");
        throw std::runtime_error("Unexpected small packet");
    }
    int n = int(buffer.at(0));
    if ((int)buffer.size() < n)
    {
        tprintf("ERROR: Packet too small according to header bytecount");
        throw std::runtime_error("Packet too small according to header bytecount");
    }
    std::string headerSerialized = buffer.substr(1, n);
    int err = 0;
    if ((err = RtDB2Serializer::deserialize(headerSerialized, header)) == RTDB2_SUCCESS)
    {
        buffer = buffer.substr(n+1);
    }
    else
    {
        tprintf("ERROR: Failed to deserialize packet header");
        throw std::runtime_error("Failed to deserialize packet header");
    }
}

void Comm::makeHeader(FrameHeader &header, int counter)
{
    header.agentId = agentId;
    header.counter = counter;
    header.timestamp = rtime::now();
}

void Comm::insertHeader(std::string &buffer, FrameHeader const &header)
{
    std::string headerSerialized;
    int err = 0;
    if ((err = RtDB2Serializer::serialize(header, headerSerialized)) == RTDB2_SUCCESS)
    {
        int n = headerSerialized.size();
        buffer = char(n) + headerSerialized + buffer;
    }
    else
    {
        throw std::runtime_error("Failed to insert header");
    }
}


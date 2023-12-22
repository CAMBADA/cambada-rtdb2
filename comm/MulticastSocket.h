#ifndef _INCLUDED_RTDB2_COMM2_MULTICASTSOCKET_HPP_
#define _INCLUDED_RTDB2_COMM2_MULTICASTSOCKET_HPP_

// JFEI Dec. 2018: based on MulticastSocket from comm v1, by Cambada

#include <arpa/inet.h>
#include <string>

class MulticastSocket
{
  public:
    MulticastSocket();
    ~MulticastSocket();

    int openSocket(std::string const &interface, std::string const &group, int port, bool loopback = false);
    int closeSocket();
    int sendData(void* data, int size);
    int receiveData(void* data, int size);

    int getSocket() const { return _socket; }
    std::string getInterface() const { return _interfaceName; }
    std::string getIpAddress() const { return _address; }

  private:
    int         _port;
    int         _socket;
    int         _interfaceIndex;
    bool        _loopback;
    std::string _interfaceName;
    std::string _address;
    std::string _group;

    struct sockaddr_in _destAddress;
    bool resolve();
    bool autoSelectInterface();
};

#endif

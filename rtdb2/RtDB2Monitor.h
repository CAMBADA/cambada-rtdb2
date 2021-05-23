#ifndef _INCLUDED_RTDB2MONITOR_H_
#define _INCLUDED_RTDB2MONITOR_H_

#include <string>
#include <vector>

class RtDB2Monitor
{
public:
    ~RtDB2Monitor();
    std::vector<std::string> getAgents();

    static RtDB2Monitor& monitor(std::string const &path);

private:
    RtDB2Monitor(std::string const &path);

    const std::string _path;
    std::vector<std::string> _agents;
    int _fd = -1; // inotify file descriptor
    int _wd = -1; // inotify watch descriptor

    bool init();
    bool modified();
    std::vector<std::string> collect();
};

#endif

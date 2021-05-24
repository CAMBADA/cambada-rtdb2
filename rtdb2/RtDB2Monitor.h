#ifndef _INCLUDED_RTDB2MONITOR_H_
#define _INCLUDED_RTDB2MONITOR_H_

#include <string>
#include <set>

class RtDB2Monitor
{
public:
    ~RtDB2Monitor();
    // Retrieve current path entries
    std::set<std::string> getPathEntries();

    static RtDB2Monitor& monitor(std::string const &path);

private:
    RtDB2Monitor(std::string const &path);

    const std::string _path;
    std::set<std::string> _path_entries;
    int _fd = -1; // inotify file descriptor
    int _wd = -1; // inotify watch descriptor

    bool init();
    bool modified();
    std::set<std::string> collect();
};

#endif

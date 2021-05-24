#include <iostream>
#include <string>
#include <sys/inotify.h>
#include <boost/filesystem.hpp>
#include "RtDB2Monitor.h"
#include "tprintf.hpp"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

namespace fs = boost::filesystem;

static std::map<std::string, RtDB2Monitor*> _monitors;

RtDB2Monitor& RtDB2Monitor::monitor(std::string const &path)
{
    std::map<std::string, RtDB2Monitor*>::iterator it = _monitors.find(path);
    if (it == _monitors.end())
    {
        //tprintf("RtDB2Monitor| Setting up new monitor for path: %s", path.c_str());
        RtDB2Monitor* monitor = new RtDB2Monitor(path);
        _monitors.insert(std::pair<std::string, RtDB2Monitor*>(path, monitor));
    }
    return *_monitors[path];
}

RtDB2Monitor::RtDB2Monitor(std::string const &path)
    : _path(path)
{
    //tprintf("RtDB2Monitor| ctor: %s", path.c_str());
    _fd = inotify_init();
    if (_fd < 0)
    {
        perror("inotify_init");
    }
}

RtDB2Monitor::~RtDB2Monitor()
{
    (void) inotify_rm_watch(_fd, _wd);
    (void) close(_fd);
}

std::set<std::string> RtDB2Monitor::getPathEntries()
{
    if (modified())
    {
        _path_entries = collect();
    }
    return _path_entries;
}

bool RtDB2Monitor::init()
{
    if (_wd > 0)
    {
        return true;
    }
    //tprintf("RtDB2Monitor| inotify_add_watch for path: %s", _path.c_str());
    _wd = inotify_add_watch(_fd, _path.c_str(),
                            IN_MODIFY | IN_CREATE | IN_DELETE);
    if (_wd < 0)
    {
        //tprintf("RtDB2Monitor| inotify_add_watch failed for path: %s", _path.c_str());
        return false;
    }
    _path_entries = collect();
    return true;
}

bool RtDB2Monitor::modified()
{
    if (!init())
    {
        return false;
    }

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(_fd, &descriptors);

    struct timeval time_to_wait;
    time_to_wait.tv_sec = 0;
    time_to_wait.tv_usec = 0;

    int return_value = select(_fd + 1, &descriptors, NULL, NULL, &time_to_wait);
    if (return_value < 0)
    {
        std::cerr << "RtDB2Monitor::modified - select() failed with result " << return_value << std::endl;
    }
    else if (!return_value)
    {
        // no change (timeout)
    }
    else if (FD_ISSET(_fd, &descriptors))
    {
        //tprintf("RtDB2Monitor| change detect on path: %s", _path.c_str());
        char buffer[BUF_LEN];
        /* Process the inotify events */
        int length = read(_fd, buffer, BUF_LEN);
        if (length < 0)
        {
            perror("read");
        }
        return true;
    }
    return false;
}

std::set<std::string> RtDB2Monitor::collect()
{
    std::set<std::string> result;
    for (const auto & entry : fs::directory_iterator(_path))
    {
        result.insert(entry.path().leaf().string());
    }
    return result;
}

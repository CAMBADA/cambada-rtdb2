// test multi-client interaction, thread safety and wait_for_put

#include <iostream>
#include <thread>
#include <unistd.h>
#include <cmath>
#include <vector>

#include "RtDB2Store.h"


class Robot
{
public:
    Robot(int agentId, char teamId)
    {
        _myId = agentId;
        _myTeam = teamId;
        std::string rtdb_path = "rtdb_team_" + std::to_string(teamId);
        RtDB2Context ctx = RtDB2Context::Builder(agentId)
            .withDatabase(rtdb_path)
            .build();
        _rtdb = new RtDB2(ctx);
        _iteration = 0;
    }
    bool tick(float sleeptime, bool synchronize)
    {
        ++_iteration;
        usleep(100000); // TODO blegh
        put();
        sleep_or_wait(sleeptime, synchronize);
        return get_and_check();
    }
    void put()
    {
        _rtdb->put("MY_INT", &_iteration);
        tprintf("put %c%d iteration %d", _myTeam, _myId, _iteration);
    }
    void sleep_or_wait(float sleeptime, bool synchronize)
    {
        if (synchronize)
        {
            tprintf("waitForPut > %c%d", _myTeam, _myId);
            _rtdb->waitForPut("HEARTBEAT", 0);
            tprintf("waitForPut < %c%d", _myTeam, _myId);
        }
        else
        {
            usleep(int(round(1e6 * sleeptime)));
        }
    }
    bool get_and_check()
    {
        bool ok = true;
        char p[280] = {0};
        sprintf(p + strlen(p), "robot %c%d iteration %4d:", _myTeam, _myId, _iteration);
        for (int r = 1; r <= 5; ++r)
        {
            if (r != _myId)
            {
                int otherValue = -1;
                int k;
                if (0 == (k = _rtdb->get("MY_INT", &otherValue, r)))
                {
                    sprintf(p + strlen(p), " %4d", otherValue);
                    ok &= (otherValue == _iteration);
                }
                else
                {
                    sprintf(p + strlen(p), " N(%d)", k);
                    ok = false;
                    
                    RtDB2Item item;
                    _rtdb->getItem("MY_INT", item, r);
                    sprintf(p + strlen(p), "[%.3f]", item.age());
                }
            }
            else
            {
                sprintf(p + strlen(p), "     ");
            }
        }
        _report = std::string(p);
        return ok;
    }
    void run(float frequency, int iterations, bool verbose, bool synchronize)
    {
        _ok = true;
        while (iterations--)
        {
            float sleeptime = 1.0 / frequency;
            // TODO: add some jitter to sleeptime?
            _ok &= tick(sleeptime, synchronize);
            if (verbose) report();
        }
    }
    void report()
    {
        tprintf("%s", _report.c_str());
    }
    bool ok()
    {
        return _ok;
    }
private:
    int _myId;
    char _myTeam;
    RtDB2 *_rtdb;
    int _iteration;
    std::string _report;
    bool _ok;
};

class Team
{
public:
    Team(char teamId, float frequency, int iterations, bool verbose, bool synchronize)
    {
        _teamId = teamId;
        for (int r = 1; r <= 5; ++r)
        {
            auto robot = new Robot(r, teamId);
            _robots.push_back(robot);
            // spawn a thread for this robot
            _threads.push_back(std::thread(&Robot::run, robot, frequency, iterations, verbose, synchronize));
        }
        // spawn a thread for heartbeat
        if (synchronize)
        {
            // sleep a bit to 'ensure' all robots are in waiting mode, otherwise they might run out of sync
            usleep(100000);
            std::string rtdb_path = "rtdb_team_" + std::to_string(teamId);
            RtDB2Context ctx = RtDB2Context::Builder(0)
                .withDatabase(rtdb_path)
                .build();
            _rtdb = new RtDB2(ctx);
            _threads.push_back(std::thread(&Team::heartbeat, this, frequency, iterations));
        }
    }
    void heartbeat(float frequency, int iterations)
    {
        while (iterations--)
        {
            float sleeptime = 1.0 / frequency;
            // TODO: add some jitter to sleeptime?
            usleep(int(round(1e6 * sleeptime)));
            int dummy = 0; // value does not matter
            tprintf("HB > %c", _teamId);
            _rtdb->put("HEARTBEAT", &dummy);
            tprintf("HB < %c", _teamId);
        }
    }
    void wait()
    {
        for (auto &t: _threads)
        {
            t.join();
        }
    }
    bool ok()
    {
        bool ok = true;
        for (auto &r: _robots)
        {
            ok &= r->ok();
        }
        return ok;
    }
private:
    std::vector<Robot *> _robots;
    std::vector<std::thread> _threads;
    RtDB2 *_rtdb; // optional, for heartbeat
    char _teamId;
};



int main(int argc, char **argv)
{
    // TODO argument parsing
    float frequency = 3.0;
    int iterations = 1;//3;
    bool verbose = true;
    bool synchronize = true;

    // construct teams, one thread per robot
    Team a = Team('A', frequency, iterations, verbose, synchronize);
    //Team b = Team('B', frequency, iterations, verbose, synchronize);
    
    // wait, join all threads
    a.wait();
    //b.wait();

    // report
    std::cout << "team A ok: " << a.ok() << std::endl;
    //std::cout << "team B ok: " << b.ok() << std::endl;

    // done
    return 0;
}


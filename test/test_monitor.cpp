#include <iostream>
#include <string>
#include "RtDB2.h"
#include "RtDB2Context.h"
#include "RtDB2Monitor.h"

void showDatabases(std::string const &dbpath)
{
    RtDB2Monitor& m = RtDB2Monitor::monitor(dbpath);
    for (auto &p : m.getPathEntries())
    {
        std::cout << "database: " << p << std::endl;
    }
}

void showAgents(RtDB2 const &rtdb)
{
    for (auto &i : rtdb.getAgentIds())
    {
        std::cout << "agent: " << i << std::endl;
    }
}

int main(int argc, char **argv)
{
    int i = 0;
    RtDB2Context ctx = RtDB2Context::Builder(100).build();

    // path does not exists, monitor skips initialization
    std::cout << "=== no agents, no dbpath ===" << std::endl;
    showDatabases(ctx.getDatabasePath());

    // monitor initializes
    std::cout << "=== agent 100 ===" << std::endl;
    RtDB2 rtdb(ctx);
    rtdb.get("KEY1", &i); // retrieve phony key to create database
    showDatabases(ctx.getDatabasePath());
    showAgents(rtdb);

    // monitor detects change
    std::cout << "=== agents 100, 101 ===" << std::endl;
    RtDB2 rtdb101(ctx, 101);
    rtdb101.get("KEY1", &i); // retrieve phone key to create database
    showDatabases(ctx.getDatabasePath());
    showAgents(rtdb);
    showAgents(rtdb101);


    RtDB2Context ctxOther = RtDB2Context::Builder(79).build();

    std::cout << "=== other client ===" << std::endl;
    RtDB2 rtdbOther(ctxOther);
    rtdbOther.get("KEY1", &i); // retrieve phony key to create database
    showDatabases(ctxOther.getDatabasePath());
    showAgents(rtdbOther);
    showDatabases(ctx.getDatabasePath());
    showAgents(rtdb);


    // done
    return 0;
}

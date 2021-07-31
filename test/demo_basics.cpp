#include <iostream>
#include "RtDB2.h"

// test structs
#include "robot.hpp"

void writeRtDB(const RtDB2Context& ctx, bool alive, float x, float y, const std::string& intention)
{
    auto rtdb = new RtDB2(ctx);

    // write a struct
    Robot robot_put;
    robot_put.alive = alive;
    robot_put.pos.x = x;
    robot_put.pos.y = y;
    robot_put.intention = intention;
    std::cout << "writing struct data into RTDB:" << std::endl;
    std::cout << robot_put << std::endl;
    rtdb->put("ROBOT_DATA", &robot_put);
    
    // write some more simple items
    int i = 37;
    rtdb->put("TEST_INT", &i);
    float f = 1.0;
    rtdb->put("TEST_FLOAT", &f);
    std::string s = "hi!!";
    rtdb->put("TEST_STRING", &s);
}

void readRtDB(const RtDB2Context& ctx)
{
    auto rtdb = new RtDB2(ctx);

    // read a struct
    Robot robot_get;
    rtdb->get("ROBOT_DATA", &robot_get);
    std::cout << "reading struct data from RTDB:" << std::endl;
    std::cout << robot_get << std::endl;
}

int main(int argc, char **argv)
{
    // no argument parsing

    // setup a RTDB database (default)
    int agentId = 2; // arbitrary
    RtDB2Context ctx = RtDB2Context::Builder(agentId).build();

    // write data
    writeRtDB(ctx, true, 1.1, 2.5, std::string("WIN"));

    // sleep a bit
    std::cout << "sleeping a bit ..." << std::endl;
    sleep(1);

    // read data
    readRtDB(ctx);

    // done
    return 0;
}

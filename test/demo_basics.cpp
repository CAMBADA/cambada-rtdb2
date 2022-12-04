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
    int agentId0 = 2; // arbitrary
    RtDB2Context ctx0 = RtDB2Context::Builder(agentId0).build();

    // setup a RTDB database (otherdb)
    int agentId1 = 711; // arbitrary
    RtDB2Context ctx1 = RtDB2Context::Builder(agentId1).withoutConfigFile().withDatabase("otherdb").build();

    // write data
    writeRtDB(ctx0, true, 1.1, 2.5, std::string("WIN"));
    writeRtDB(ctx1, false, -5.43, 3.14, std::string("SCORE"));

    // sleep a bit
    std::cout << "sleeping a bit ..." << std::endl;
    sleep(1);

    // read data
    readRtDB(ctx0);
    readRtDB(ctx1);

    // done
    return 0;
}

#include <iostream>
#include "RtDB2.h"

// test structs
#include "robot.hpp"


int main(int argc, char **argv)
{
    // no argument parsing
    
    // setup a RTDB database
    int agentId = 2; // arbitrary
    auto rtdb = new RtDB2(agentId);
    
    // write a struct
    Robot robot_put;
    robot_put.alive = true;
    robot_put.pos.x = 1.1;
    robot_put.pos.y = 2.5;
    robot_put.intention = "WIN";
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
    
    // sleep a bit
    std::cout << "sleeping a bit ..." << std::endl;
    sleep(1);

    // read a struct
    Robot robot_get;
    rtdb->get("ROBOT_DATA", &robot_get);
    std::cout << "reading struct data from RTDB:" << std::endl;
    std::cout << robot_get << std::endl;

    // TODO: a built-in recursive json formatter would be nice?
    // then we could just dump the entire struct:
    //std::cout << robot_get << std::endl;
    

    // done
    return 0;
}


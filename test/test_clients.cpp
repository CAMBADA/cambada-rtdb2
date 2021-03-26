#include <libgen.h>
#include <iostream>
#include "RtDB2.h"
#include "../comm/comm.hpp"

#define ROOTPATH1 RTDB2_DEFAULT_PATH "/comm1"
#define ROOTPATH2 RTDB2_DEFAULT_PATH "/comm2"
#define CONFIGFILE "/config/test_clients.xml"

// Simulates two clients sharing their database with the other client.
// Each client uses a private storage path to store its own database and the
// database shared by the other client.
// Each client has its own comm process that distributes the database over the
// network to the other comm process.

int main(int argc, char **argv)
{
    // no argument parsing

    // determine path of binary
    char buffer[PATH_MAX];
    realpath(argv[0], buffer);
    char *path = dirname(buffer);
    // compose configuration file name
    std::string configFileName(path);
    configFileName += CONFIGFILE;
    std::cout << "Using configuration file: " << configFileName << std::endl;

    // setup comm for client 1
    RtDB2Context ctxComm1 = RtDB2Context::Builder(RtDB2ProcessType::comm)
                               .withConfigFileName(configFileName)
                               .withRootPath(ROOTPATH1)
                               .build();
    Comm comm1(1, ctxComm1);
    boost::thread t1 = comm1.start();

    sleep(1); // configuration is not multi-thread safe

    // setup comm for client 2
    RtDB2Context ctxComm2 = RtDB2Context::Builder(RtDB2ProcessType::comm)
                               .withConfigFileName(configFileName)
                               .withRootPath(ROOTPATH2)
                               .build();
    Comm comm2(2, ctxComm2);
    boost::thread t2 = comm2.start();

    sleep(1); // configuration is not multi-thread safe

    // client 1
    RtDB2Context ctxNode1 = RtDB2Context::Builder()
                               .withConfigFileName(configFileName)
                               .withRootPath(ROOTPATH1)
                               .build();
    RtDB2 rtdbNode1Client1RW(1, ctxNode1);
    RtDB2 rtdbNode1Client2R (2, ctxNode1);

    sleep(1); // configuration is not multi-thread safe

    // client 2
    RtDB2Context ctxNode2 = RtDB2Context::Builder()
                               .withConfigFileName(configFileName)
                               .withRootPath(ROOTPATH2)
                               .build();
    RtDB2 rtdbNode2Client1R (1, ctxNode2);
    RtDB2 rtdbNode2Client2RW(2, ctxNode2);

    // rtdb
    srand (time(NULL));
    const std::string key = "EXAMPLE_ITEM";
    int value;
    int result;

    result = rtdbNode1Client1RW.get(key, &value);
    std::cout << "Node 1 - Client 1 (success: " << result << "): " << value << std::endl;
    result = rtdbNode1Client2R.get(key, &value);
    std::cout << "Node 1 - Client 2 (success: " << result << "): " << value << std::endl;
    result = rtdbNode2Client1R.get(key, &value);
    std::cout << "Node 2 - Client 1 (success: " << result << "): " << value << std::endl;
    result = rtdbNode2Client2RW.get(key, &value);
    std::cout << "Node 2 - Client 2 (success: " << result << "): " << value << std::endl;

    int value1 = rand() % 1000;
    std::cout << "Writing value " << value1 << " to Node 1 - Client 1" << std::endl;
    rtdbNode1Client1RW.put(key, &value1);
    int value2 = rand() % 1000;
    std::cout << "Writing value " << value2 << " to Node 2 - Client 2" << std::endl;
    rtdbNode2Client2RW.put(key, &value2);

    usleep(300000);

    result = rtdbNode1Client1RW.get(key, &value);
    std::cout << "Node 1 - Client 1 (success: " << result << "): " << value << std::endl;
    result = rtdbNode1Client2R.get(key, &value);
    std::cout << "Node 1 - Client 2 (success: " << result << "): " << value << std::endl;
    result = rtdbNode2Client1R.get(key, &value);
    std::cout << "Node 2 - Client 1 (success: " << result << "): " << value << std::endl;
    result = rtdbNode2Client2RW.get(key, &value);
    std::cout << "Node 2 - Client 2 (success: " << result << "): " << value << std::endl;

    Comm::shutdown();
    usleep(100000);
    // TODO: Shutdown blocks on receiver thread in comm
    // t1.join();
    // t2.join();

    // done
    return 0;
}

#include <iostream>
#include "RtDB2Configuration.h"

void print_comm_settings(RtDB2Configuration& config) {
    CommunicationSettings cs = config.get_communication_settings();
    std::cout << "\tmultiCastIP = " << cs.multiCastIP << std::endl;
    std::cout << "\tfrequency   = " << cs.frequency << std::endl;
    std::cout << "\tport        = " << cs.port << std::endl;
    std::cout << "\tcompression = " << cs.compression << std::endl;
    std::cout << "\tloopback    = " << cs.loopback << std::endl;
    std::cout << "\tsend        = " << cs.send << std::endl;
}

int main(int argc, char **argv)
{
    std::cout << "=== Default configuration ===" << std::endl;
    RtDB2Configuration config1;
    print_comm_settings(config1);
    std::cout << config1 << std::endl;

    std::cout << "=== Minimal configuration ===" << std::endl;
    RtDB2Configuration config2;
    config2.parse_configuration("config/minimal_configuration.xml");
    print_comm_settings(config2);
    std::cout << config2 << std::endl;

    std::cout << "=== Extensive configuration ===" << std::endl;
    RtDB2Configuration config3;
    config3.parse_configuration("config/extensive_configuration.xml");
    print_comm_settings(config3);
    std::cout << config3 << std::endl;

    // done
    return 0;
}


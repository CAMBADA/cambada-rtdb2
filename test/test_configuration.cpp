#include <iostream>
#include "RtDB2Configuration.h"

void print_comm_settings(RtDB2Configuration &config)
{
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

    for (int i = 1; i < argc; i++)
    {
        std::cout << "=== " << argv[i] << " ===" << std::endl;
        RtDB2Configuration config;
        config.parse_configuration(argv[i]);
        print_comm_settings(config);
        std::cout << config << std::endl;
    }

    std::cout << "=== Minimal configuration v2 ===" << std::endl;
    RtDB2Configuration config4;
    config4.parse_configuration("config/minimal_configuration_v2.xml");
    print_comm_settings(config4);
    std::cout << config4 << std::endl;

    std::cout << "=== Extensive configuration v2 ===" << std::endl;
    RtDB2Configuration config5;
    config5.parse_configuration("config/extensive_configuration_v2.xml");
    print_comm_settings(config5);
    std::cout << config5 << std::endl;

    // done
    return 0;
}

#include <iostream>
#include "RtDB2Context.h"

void print_configuration(const RtDB2Configuration &config)
{
    CommunicationSettings cs = config.get_communication_settings();
    std::cout << "\tmultiCastIP = " << cs.multiCastIP << std::endl;
    std::cout << "\tfrequency   = " << cs.frequency << std::endl;
    std::cout << "\tport        = " << cs.port << std::endl;
    std::cout << "\tloopback    = " << cs.loopback << std::endl;
    std::cout << "\tsend        = " << cs.send << std::endl;
    std::cout << config << std::endl;
}

int main(int argc, char **argv)
{
    std::cout << "=== Default configuration ===" << std::endl;
    RtDB2Context defaultContext = RtDB2Context::Builder(0).build();
    print_configuration(defaultContext.getConfiguration());

    for (int i = 1; i < argc; i++)
    {
        std::string configFile = argv[i];
        std::cout << "=== " << configFile << " ===" << std::endl;
        RtDB2Context context = RtDB2Context::Builder(0, RtDB2ProcessType::comm)
                                   .withConfigFileName(configFile)
                                   .build();
        print_configuration(context.getConfiguration());
    }

    // done
    return 0;
}

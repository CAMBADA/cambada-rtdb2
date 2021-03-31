#include <iostream>
#include <string>
#include "RtDB2Context.h"

int main(int argc, char **argv)
{
    std::cout << "=== default context ===" << std::endl;
    RtDB2Context ctx = RtDB2Context::Builder(0).build();
    std::cout << ctx << std::endl;

    std::cout << "=== default dbclient context ===" << std::endl;
    ctx = RtDB2Context::Builder(0, RtDB2ProcessType::dbclient).build();
    std::cout << ctx << std::endl;

    std::cout << "=== default comm context ===" << std::endl;
    ctx = RtDB2Context::Builder(0, RtDB2ProcessType::comm).build();
    std::cout << ctx << std::endl;

    std::cout << "=== comm context ===" << std::endl;
    ctx = RtDB2Context::Builder(0, RtDB2ProcessType::comm)
            .withConfigFileName("config/context_myconfig.xml")
            .withNetwork("mynetwork")
            .withRootPath("myrootpath")
            .build();
    std::cout << ctx << std::endl;

    std::cout << "=== database not allowed for comm ===" << std::endl;
    ctx = RtDB2Context::Builder(1, RtDB2ProcessType::comm)
            .withDatabase("mydatabase")
            .build();
    std::cout << ctx << std::endl;

    std::cout << "=== network not allow for dbclient ===" << std::endl;
    ctx = RtDB2Context::Builder(2)
            .withNetwork("mynetwork")
            .build();
    std::cout << ctx << std::endl;

    // done
    return 0;
}

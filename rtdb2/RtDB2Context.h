#ifndef _INCLUDED_RTDB2CONTEXT_H_
#define _INCLUDED_RTDB2CONTEXT_H_

#include <string>
#include "RtDB2Definitions.h"

enum RtDB2ProcessType
{
    comm,
    dbclient
};

inline const char *toString(RtDB2ProcessType t)
{
    switch (t)
    {
    case comm:
        return "comm";
    case dbclient:
        return "dbclient";
    default:
        return "[Unknown ProcessType]";
    }
}

class RtDB2Context
{
public:
    class Builder;

    RtDB2ProcessType getProcessType();
    std::string getConfigFileName();
    std::string getNetworkName();
    std::string getDatabaseName();
    std::string getRootPath();

    friend std::ostream& operator<<(std::ostream& os, RtDB2Context& ctx);

    static std::string defaultConfigFileName();

private:
    RtDB2Context(
        RtDB2ProcessType processType,
        std::string const &networkName,
        std::string const &databaseName,
        std::string const &rootPath,
        std::string const &configFileName);

    std::ostream& toStream(std::ostream&);

    RtDB2ProcessType _processType;
    std::string _networkName;
    std::string _databaseName;
    std::string _rootPath;
    std::string _configFileName;

};

inline std::ostream& operator<<(std::ostream& os, RtDB2Context& ctx) {
    ctx.toStream(os);
    return os;
}

class RtDB2Context::Builder
{
public:
    Builder(RtDB2ProcessType processType = RtDB2ProcessType::dbclient);
    Builder &withNetwork(std::string const &networkName);
    Builder &withDatabase(std::string const &databaseName);
    Builder &withRootPath(std::string const &rootPath);
    Builder &withConfigFileName(std::string const &configFileName);
    RtDB2Context build() const;

private:
    RtDB2ProcessType _processType = RtDB2ProcessType::dbclient;
    std::string _networkName = "default";
    std::string _databaseName = "default";
    std::string _rootPath = RTDB2_DEFAULT_PATH;
    std::string _configFileName;
};

#endif

#ifndef _INCLUDED_RTDB2CONTEXT_H_
#define _INCLUDED_RTDB2CONTEXT_H_

#include <string>
#include "RtDB2Definitions.h"
#include "RtDB2Configuration.h"

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
    const RtDB2Configuration &getConfiguration();
    std::string getConfigFileName();
    std::string getNetworkName();
    std::string getDatabaseName();
    std::string getRootPath();

    friend std::ostream& operator<<(std::ostream& os, const RtDB2Context& ctx);

    static std::string defaultConfigFileName();

private:
    RtDB2Context(
        RtDB2ProcessType const &processType,
        std::string const &networkName,
        std::string const &databaseName,
        std::string const &rootPath,
        std::string const &configFileName,
        RtDB2Configuration const &configuration);

    std::ostream& toStream(std::ostream&) const;

    RtDB2ProcessType _processType;
    std::string _networkName;
    std::string _databaseName;
    std::string _rootPath;
    std::string _configFileName;
    RtDB2Configuration _configuration;
};

inline std::ostream& operator<<(std::ostream& os, const RtDB2Context& ctx) {
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

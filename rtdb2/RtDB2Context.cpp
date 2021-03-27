#include "RtDB2Context.h"
#include <iostream>
#include <stdexcept>

std::string RtDB2Context::defaultConfigFileName()
{
    char *cp = NULL;
    if ((cp = getenv("RTDB_CONFIG_PATH")) != NULL)
    {
        return std::string(cp) + "/rtdb2_configuration.xml";
    }
    return RTDB2_CONFIGURATION_FILE;
}

RtDB2Context::RtDB2Context(
    RtDB2ProcessType const &processType,
    std::string const &rootPath,
    std::string const &configFileName,
    RtDB2Configuration const &configuration)
    : _processType(processType), _rootPath(rootPath), _configFileName(configFileName), _configuration(configuration)
{
}

std::string RtDB2Context::getConfigFileName() const
{
    return _configFileName;
}

const RtDB2Configuration &RtDB2Context::getConfiguration() const
{
    return _configuration;
}

std::string RtDB2Context::getNetworkName() const
{
    return _configuration.get_network_name();
}

std::string RtDB2Context::getDatabaseName() const
{
    return _configuration.get_database_name();
}

RtDB2ProcessType RtDB2Context::getProcessType() const
{
    return _processType;
}

std::string RtDB2Context::getRootPath() const
{
    return _rootPath;
}

std::ostream &RtDB2Context::toStream(std::ostream &os) const
{
    os << "processType: " << toString(_processType) << std::endl;
    os << "rootPath: " << _rootPath << std::endl;
    os << "configFileName: " << _configFileName << std::endl;
    os << "networkName: " << getNetworkName() << std::endl;
    os << "databaseName: " << getDatabaseName() << std::endl;
    return os;
}

// --- Builder ---

RtDB2Context::Builder::Builder(RtDB2ProcessType processType)
{
    _processType = processType;
    _configFileName = defaultConfigFileName();
}

RtDB2Context::Builder &RtDB2Context::Builder::withNetwork(std::string const &networkName)
{
    if (_processType == RtDB2ProcessType::comm)
    {
        _networkName = networkName;
    }
    else
    {
        std::cout << "[WARNING] Network can only be set for comm processes" << std::endl;
    }
    return *this;
}

RtDB2Context::Builder &RtDB2Context::Builder::withDatabase(std::string const &databaseName)
{
    if (_processType == RtDB2ProcessType::comm)
    {
        std::cout << "[WARNING] Database cannot be set on comm processes. "
                  << "Comm processes use database set in Network configuration." << std::endl;
    }
    else
    {
        _databaseName = databaseName;
    }
    return *this;
}

RtDB2Context::Builder &RtDB2Context::Builder::withRootPath(std::string const &rootPath)
{
    _rootPath = rootPath;
    return *this;
}

RtDB2Context::Builder &RtDB2Context::Builder::withConfigFileName(std::string const &configFileName)
{
    _configFileName = configFileName;
    return *this;
}

RtDB2Context RtDB2Context::Builder::build() const
{
    // process type comm extracts the database name from the network config
    std::string db = _processType == RtDB2ProcessType::comm ? "" : _databaseName;
    std::string netw = _processType == RtDB2ProcessType::dbclient ? "" : _networkName;
    RtDB2Configuration configuration(_configFileName, _processType, db, netw);
    configuration.load_configuration();
    return RtDB2Context(_processType, _rootPath, _configFileName, configuration);
}

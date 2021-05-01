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
    int agent,
    RtDB2ProcessType const &processType,
    std::string const &rootPath,
    std::string const &configFileName,
    RtDB2Configuration const &configuration)
    : _agent(agent), _processType(processType), _rootPath(rootPath), _configFileName(configFileName), _configuration(configuration)
{
}

int RtDB2Context::getAgentId() const
{
    return _agent;
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

std::string RtDB2Context::getDatabasePath() const
{
    return _rootPath + '/' + std::to_string(_agent) + '/' + _configuration.get_database_name();
}

std::ostream &RtDB2Context::toStream(std::ostream &os) const
{
    os << "me: " << std::to_string(_agent) << std::endl;
    os << "processType: " << toString(_processType) << std::endl;
    os << "rootPath: " << _rootPath << std::endl;
    os << "configFileName: " << _configFileName << std::endl;
    os << "networkName: " << getNetworkName() << std::endl;
    os << "databaseName: " << getDatabaseName() << std::endl;
    os << "databasePath: " << getDatabasePath() << std::endl;
    return os;
}

// --- Builder ---

RtDB2Context::Builder::Builder(int agent, RtDB2ProcessType processType)
    : _agent(agent)
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
    if (_processType == RtDB2ProcessType::comm && configFileName.compare("") == 0)
    {
        std::cout << "[WARNING] Configuration file is required for Comm processes." << std::endl;
    }
    else
    {
        _configFileName = configFileName;
    }
    return *this;
}

RtDB2Context::Builder &RtDB2Context::Builder::withoutConfigFile()
{
    return withConfigFileName("");
}

RtDB2Context RtDB2Context::Builder::build() const
{
    if (_configFileName.compare("") == 0 && _databaseName.compare("default") != 0)
    {
        std::cout << "[WARNING] Invalid context. Configuration file is required for non-default database." << std::endl;
    }
    // process type comm extracts the database name from the network config
    std::string db = _processType == RtDB2ProcessType::comm ? "" : _databaseName;
    std::string netw = _processType == RtDB2ProcessType::dbclient ? "" : _networkName;
    RtDB2Configuration configuration(_configFileName, _processType, db, netw);
    configuration.load_configuration();
    return RtDB2Context(_agent, _processType, _rootPath, _configFileName, configuration);
}

#ifndef CAMBADA_RTDB2KEYS_H
#define CAMBADA_RTDB2KEYS_H

#include <map>

#include "RtDB2Definitions.h"

enum class RtDB2ProcessType
{
    comm,
    dbclient
};

struct KeyDetail {
    KeyDetail() : shared(true), period(1), phase_shift(0), timeout(1.0) {}
    KeyDetail(const KeyDetail& defaults) :
            shared(defaults.shared), period(defaults.period), phase_shift(defaults.phase_shift), timeout(defaults.timeout) {}
    bool shared;
    int period;
    int phase_shift;
    float timeout;
};

struct CompressorSettings {
    CompressorSettings() : name("zstd"), use_dictionary(false) {}
    CompressorSettings(std::string name, bool use_dictionary) :
            name(name), use_dictionary(use_dictionary) {}
    std::string name;
    bool use_dictionary;
};

struct CommunicationSettings
{
    std::string multiCastIP = "";
    std::string interface = "auto";
    std::vector<std::string> interfacePriorityList;
    std::set<std::string> interfaceBlackList;
    int port = 8001;
    float frequency = 30;
    bool loopback = false;
    bool send = true;
    bool diagnostics = true;
};

class RtDB2Configuration
{
public:
    RtDB2Configuration(
        std::string const &configFile,
        RtDB2ProcessType const &processType,
        std::string const &database,
        std::string const &network);

    void load_configuration();
    const std::string get_network_name() const { return network_; };
    const std::string get_database_name() const { return database_; };
    const KeyDetail &get_key_default() const;
    const KeyDetail &get_key_details(const std::string &id) const;
    const KeyDetail &get_key_details(const int &oid) const;
    const std::string *get_string_identifier(const int &oid) const;
    const CompressorSettings &get_compressor_settings() const;
    const CommunicationSettings &get_communication_settings() const;

    friend std::ostream &operator<<(std::ostream &os, const RtDB2Configuration &obj);

private:
    int parse_configuration(std::string file_path);
    void associate_keys_int_string(int oid, std::string id);
    void insert_key_detail(std::string id, KeyDetail detail);

    std::string configFile_;
    RtDB2ProcessType processType_;
    std::string database_;
    std::string network_;
    std::map<std::string, KeyDetail> keys_details_;
    std::map<int, std::string> reverse_key_map_;
    KeyDetail default_details_;
    CompressorSettings compressor_settings_;
    CommunicationSettings communication_settings_;
};

#endif //CAMBADA_RTDB2KEYS_H

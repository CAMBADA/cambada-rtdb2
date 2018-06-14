#ifndef CAMBADA_RTDB2KEYS_H
#define CAMBADA_RTDB2KEYS_H

#include <map>

#include "RtDB2Definitions.h"

struct KeyDetail {
    KeyDetail() : shared(true), period(1), phase_shift(0) {}
    KeyDetail(const KeyDetail& defaults) :
            shared(defaults.shared), period(defaults.period), phase_shift(defaults.phase_shift) {}
    bool shared;
    int period;
    int phase_shift;
};

struct CompressorSettings {
    CompressorSettings() : name("zstd"), use_dictionary(true) {}
    CompressorSettings(std::string name, bool use_dictionary) :
            name(name), use_dictionary(use_dictionary) {}
    std::string name;
    bool use_dictionary;
};

class RtDB2Configuration {
public:
    RtDB2Configuration();

    int parse_configuration(std::string file_path = RTDB2_CONFIGURATION_FILE);
    const KeyDetail& get_key_default() const;
    const KeyDetail& get_key_details(const std::string& id) const;
    const KeyDetail& get_key_details(const int& oid) const;
    const std::string* get_string_identifier(const int& oid) const;
    const CompressorSettings& get_compressor_settings() const;

    friend std::ostream& operator<<(std::ostream& os, RtDB2Configuration& obj);
private:
    void associate_keys_int_string(int oid, std::string id);
    void insert_key_detail(std::string id, KeyDetail detail);


    std::map<std::string, KeyDetail> keys_details_;
    std::map<int, std::string> reverse_key_map_;
    KeyDetail default_details_;
    CompressorSettings compressor_settings_;
};


#endif //CAMBADA_RTDB2KEYS_H

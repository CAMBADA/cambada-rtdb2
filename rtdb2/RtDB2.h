#ifndef CAMBADA_RtDB2_H
#define CAMBADA_RtDB2_H

#include <string>
#include <map>
#include <limits>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include <sys/time.h>

#include "RtDB2Definitions.h"
#include "RtDB2ErrorCode.h"
#include "RtDB2Configuration.h"

#include "serializer/RtDB2Serializer.h"
#include "compressor/RtDB2Compressor.h"
#include "storage/RtDB2StorageLMDB.h"

class RtDB2 {
public:
    RtDB2(int db_identifier);
    RtDB2(int db_identifier, std::string path);

    // This function corresponds to the expected behaviour of a put function
    // Where the agent is allowed to store information to send or to stay local
    template <typename T>
    int put(std::string key, T* value);
    template <typename T>
    int put(int key, T* value);
    int put_batch(int db_identifier, const std::string& batch, int life, bool shared = true, bool is_compressed = true);
    // This function corresponds to a put where the process is allowed to create
    // information replicated from the other agents, at the moment this is only
    // used by simulator. Maybe simulator should use put_batch and get_batch
    // instead.
    template <typename T>
    int put_root(std::string key, T* value, int life, int db_dst);
    template <typename T>
    int put_root(int key, T* value, int life, int db_dst);

    // This functions allows the agent to obtain information from other agents
    template <typename T>
    int get(std::string key, T* value, int& life, int db_src);
    template <typename T>
    int get(int key, T* value, int& life, int db_src);
    int get_batch(std::string& batch, bool exclude_local = true, bool compress = true);

    const RtDB2Configuration& get_configuration() const;
private:
    template <typename T>
    int put_core(std::string key, T* value, int life, int db_dst);
    std::string create_agent_name(int db_identifier, bool shared = true);
    boost::shared_ptr<RtDB2Storage>& get_remote_ptr(int db_identifier);
    void construct_priv();

    void put_timestamp(std::string& dst, int life);
    int get_and_remove_timestamp(std::string& src);

    const int db_identifier_;
    std::string path_;
    RtDB2Configuration configuration_;

    std::map<int, boost::shared_ptr<RtDB2Storage> > remote_;
    boost::shared_ptr<RtDB2Storage> local_;
    boost::shared_ptr<RtDB2Compressor> compressor_;

    unsigned int batch_counter_;
};

template<typename T>
int RtDB2::put_core(std::string key, T* value, int life, int db_dst) {
    if (value == NULL) return RTDB2_VALUE_POINTING_TO_NULL;

    std::string serialized_data;
    int err = RtDB2Serializer::serialize(*value, serialized_data);
    if (err) return err;

    put_timestamp(serialized_data, life);
    bool shared = configuration_.get_key_details(key).shared;
    if (shared) {
        get_remote_ptr(db_dst)->insert(key, serialized_data);
    } else {
        local_->insert(key, serialized_data);
    }
    return RTDB2_SUCCESS;
}

template<typename T>
int RtDB2::put(std::string key, T* value) {
    return put_core(key, value, 0, db_identifier_);
}

template <typename T>
int RtDB2::put(int key, T* value) {
    const std::string* id = configuration_.get_string_identifier(key);
    if (id == NULL) return RTDB2_INTEGER_ID_NOT_FOUND;
    return put(*id, value);
}

template<typename T>
int RtDB2::put_root(std::string key, T* value, int life, int db_dst) {
    return put_core(key, value, life, db_dst);
}

template <typename T>
int RtDB2::put_root(int key, T* value, int life, int db_dst) {
    const std::string* id = configuration_.get_string_identifier(key);
    if (id == NULL) return RTDB2_INTEGER_ID_NOT_FOUND;
    return put_root(*id, value, life, db_dst);
}

template<typename T>
int RtDB2::get(std::string key, T* value, int& life, int db_from) {
    int err;
    std::string raw_data;

    bool found_local = false;
    if (db_from == db_identifier_) {
        err = local_->fetch(key, raw_data);
        if (err == RTDB2_SUCCESS)
            found_local = true;
    }

    if (!found_local) {
        err = get_remote_ptr(db_from)->fetch(key, raw_data);
        if (err != RTDB2_SUCCESS) return err;
    }

    life = get_and_remove_timestamp(raw_data);
    if (value != NULL) {
        err = RtDB2Serializer::deserialize(raw_data, *value);
        if (err) return err;
    }
    return RTDB2_SUCCESS;
}

template <typename T>
int RtDB2::get(int key, T* value, int& life, int db_src) {
    const std::string* id = configuration_.get_string_identifier(key);
    if (id == NULL)
        return RTDB2_KEY_NOT_FOUND;
    return get(*id, value, life, db_src);
}

#endif //CAMBADA_RtDB2_H

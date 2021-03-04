#include "RtDB2.h"
#include <iostream>

#include "compressor/RtDB2CompressorZstd.h"
#include "compressor/RtDB2CompressorLZ4.h"

void RtDB2::construct_priv() 
{
    const CompressorSettings compressor_settings_ = configuration_.get_compressor_settings();
    if (compressor_settings_.name == "lz4") {
        compressor_ = boost::make_shared<RtDB2CompressorLZ4>();
    } else if (compressor_settings_.name == "zstd") {
        if (compressor_settings_.use_dictionary) {
            compressor_ = boost::make_shared<RtDB2CompressorZstd>(ZSTD2_DICTIONARY_FILE);
        } else {
            compressor_ = boost::make_shared<RtDB2CompressorZstd>();
        }
    } else {
        std::cout << "Invalid compressor name (" << compressor_settings_.name
                  << ") was received into the RtDB" << std::endl
                  << "Considering default values (zstd with dictionary)" << std::endl;
        compressor_ = boost::make_shared<RtDB2CompressorZstd>(ZSTD2_DICTIONARY_FILE);
    }
}

RtDB2::RtDB2(int db_identifier) :
        db_identifier_(db_identifier), path_(DEFAULT_PATH), configuration_(),
        local_(boost::make_shared<RtDB2LMDB>(DEFAULT_PATH, create_agent_name(db_identifier, false))),
        //compressor_(NULL), c++11 only
        batch_counter_(0)
{
    construct_priv();
}

RtDB2::RtDB2(int db_identifier, std::string path) :
        db_identifier_(db_identifier), path_(path), configuration_(),
        local_(boost::make_shared<RtDB2LMDB>(path, create_agent_name(db_identifier, false))),
        // compressor_(NULL), c++11 only
        batch_counter_(0)
{
    construct_priv();
}

std::string RtDB2::create_agent_name(int db_identifier, bool shared) {
    std::stringstream stream;
    stream << DB_PREPEND_NAME;
    stream << "_";

    if (shared)
        stream << "shared";
    else
        stream << "local";
    stream << db_identifier;
    return stream.str();
}

boost::shared_ptr<RtDB2Storage>& RtDB2::get_remote_ptr(int db_identifier) {
    std::map<int, boost::shared_ptr<RtDB2Storage> >::iterator it = remote_.find(db_identifier);
    if (it == remote_.end()) {
        it = remote_.insert(std::pair<int, boost::shared_ptr<RtDB2Storage> >(
                db_identifier, boost::make_shared<RtDB2LMDB>(path_, create_agent_name(db_identifier)))).first;
    }
    return it->second;
}

int RtDB2::put_batch(int db_identifier, const std::string &batch, int life, bool shared, bool is_compressed) {
    int err;
    std::vector<std::pair<std::string, std::string> > data;
    std::string batch_serialized;

    if (is_compressed) {
        if ((err = compressor_->decompress(batch, batch_serialized)) != RTDB2_SUCCESS)
            return err;
    } else {
        batch_serialized.assign(batch);
    }

    if ((err = RtDB2Serializer::deserialize(batch_serialized, data)) != RTDB2_SUCCESS)
        return err;
    // Increment the difference with the input parameter to every value in the vector
    std::vector<std::pair<std::string, std::string> >::iterator it_data;
    for (it_data = data.begin(); it_data != data.end(); it_data++) {
        std::pair<std::string, std::string>& value = *it_data;
        std::string& str_value = value.second;
        int *life_stored = (int *) (str_value.c_str() + str_value.size() - sizeof(int));
        int life_calculated = life + *life_stored;

        str_value.resize(str_value.size() - sizeof(int));
        put_timestamp(value.second, life_calculated);
    }

    // Insert the batch data collected into the RtDB
    if (shared) {
        if (remote_.find(db_identifier) == remote_.end()) {
            remote_.insert(std::pair<int, boost::shared_ptr<RtDB2Storage> >(
                    db_identifier, boost::make_shared<RtDB2LMDB>(path_, create_agent_name(db_identifier))));
        }
        remote_.find(db_identifier)->second->insert_batch(data);
    } else {
        local_->insert_batch(data);
    }
    return 0;
}

int RtDB2::get_batch(std::string& batch, bool exclude_local, bool compress) {
    int err;
    std::vector<std::pair<std::string, std::string> > data;
    std::map<int, boost::shared_ptr<RtDB2Storage> >::iterator it = remote_.find(db_identifier_);
    get_remote_ptr(db_identifier_)->fetch_all_data(data);

    if (!exclude_local) {
        std::vector<std::pair<std::string, std::string> > data_tmp;
        local_->fetch_all_data(data_tmp);
        data.insert(data.end(), data_tmp.begin(), data_tmp.end());
    }

    // Skip the key if it is not the required period plus its phase
    std::vector<std::pair<std::string, std::string> >::iterator it_data;
    long int counter = static_cast<long int>(batch_counter_);
    for (it_data = data.begin(); it_data != data.end(); ) {
        const KeyDetail& detail = configuration_.get_key_details(it_data->first);
        if (counter >= detail.phase_shift && (counter - detail.phase_shift) % detail.period == 0) {
            it_data++;
        } else {
            it_data = data.erase(it_data);
        }
    }

    // Compute the differences and insert that life value again
    for (it_data = data.begin(); it_data != data.end(); it_data++) {
        std::pair<std::string, std::string>& value = *it_data;
        std::string& str_value = value.second;

        int life = get_and_remove_timestamp(str_value);
        str_value.resize(str_value.size() + sizeof(int));
        memcpy((void*) (str_value.c_str() + str_value.size() - sizeof(int)), &life, sizeof(int));
    }

    std::string batch_serialized;
    if ((err = RtDB2Serializer::serialize(data, batch_serialized)) != RTDB2_SUCCESS)
        return err;

    if (compress) {
        if ((err = compressor_->compress(batch_serialized, batch)) != RTDB2_SUCCESS)
            return err;
    } else {
        batch.assign(batch_serialized);
    }
    batch_counter_ += 1;
    return 0;
}

void RtDB2::put_timestamp(std::string &dst, int life) {
    struct timeval time;
    gettimeofday(&time, NULL);

    long int life_sec = time.tv_sec - life / 1000;
    long int life_usec = time.tv_usec - (life % 1000) * 1000;

    dst.resize(dst.size() + sizeof(life_sec));
    memcpy((void*) (dst.c_str() + dst.size() - sizeof(long int)), &life_sec, sizeof(long int));
    dst.resize(dst.size() + sizeof(life_usec));
    memcpy((void*) (dst.c_str() + dst.size() - sizeof(long int)), &life_usec, sizeof(long int));
}

int RtDB2::get_and_remove_timestamp(std::string& src) {
    long int *life_sec = (long int *) (src.c_str() + src.size() - sizeof(long int) * 2);
    long int *life_usec = (long int *) (src.c_str() + src.size() - sizeof(long int));

    struct timeval time;
    gettimeofday(&time, NULL);

    int life = (int) ((time.tv_sec - *life_sec) * 1E3 + (time.tv_usec - *life_usec) / 1E3);

    src.resize(src.size() - sizeof(long int) * 2);
    return life;
}

const RtDB2Configuration& RtDB2::get_configuration() const {
    return configuration_;
}

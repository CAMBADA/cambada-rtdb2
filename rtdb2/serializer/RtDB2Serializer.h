#ifndef CAMBADA_RTDB2SERIALIZER_H
#define CAMBADA_RTDB2SERIALIZER_H

#include <string>
#include <msgpack.hpp>

class RtDB2Serializer {
public:
    template <typename T>
    static int serialize(const T& data, std::string& serialized_data);
    template <typename T>
    static int deserialize(const std::string& serialized_data, T& object);
};

template<typename T>
int RtDB2Serializer::serialize(const T& data, std::string& serialized_data) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, data);
    serialized_data.assign(sbuf.data(), sbuf.size());
    return RTDB2_SUCCESS;
}

template<typename T>
int RtDB2Serializer::deserialize(const std::string& serialized_data, T& object) {
    try {
        msgpack::object_handle msg = msgpack::unpack(serialized_data.data(), serialized_data.size());
        msgpack::object obj = msg.get();
        obj.convert(object);
    } catch (...) {
        return RTDB2_FAILED_DESERIALIZE;
    }
    return RTDB2_SUCCESS;
}


#endif //CAMBADA_RTDB2SERIALIZER_H

#ifndef CAMBADA_RTDB2COMPRESSOR_H
#define CAMBADA_RTDB2COMPRESSOR_H

#include <string>

class RtDB2Compressor {
public:
    virtual ~RtDB2Compressor() {}

    virtual int compress(const std::string& data, std::string& compressed) = 0;
    virtual int decompress(const std::string& compressed_data, std::string& decompressed) = 0;
};


#endif //CAMBADA_RTDB2COMPRESSOR_H

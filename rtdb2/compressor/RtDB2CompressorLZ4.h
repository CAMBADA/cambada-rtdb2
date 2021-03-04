#ifndef CAMBADA_RTDB2COMPRESSORLZ4_H
#define CAMBADA_RTDB2COMPRESSORLZ4_H

#include "RtDB2Compressor.h"

class RtDB2CompressorLZ4 : public RtDB2Compressor {
public:
    RtDB2CompressorLZ4();

    virtual int compress(const std::string& data, std::string& decompressed);
    virtual int decompress(const std::string& compressed_data, std::string& decompressed);
};


#endif //CAMBADA_RTDB2COMPRESSORLZ4_H

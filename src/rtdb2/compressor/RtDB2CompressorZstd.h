#ifndef CAMBADA_RTDB2COMPRESSORZSTD_H
#define CAMBADA_RTDB2COMPRESSORZSTD_H

#include "RtDB2Compressor.h"

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

class RtDB2CompressorZstd : public RtDB2Compressor {
public:
    RtDB2CompressorZstd();
    RtDB2CompressorZstd(const std::string& dictionary_location);
    virtual ~RtDB2CompressorZstd();

    virtual int compress(const std::string& data, std::string& compressed);
    virtual int decompress(const std::string& compressed_data, std::string& decompressed);
    void use_dictionary(const std::string& dictionary_location);

private:
    ZSTD_CDict* compressor_dict_;
    ZSTD_DDict* decompressor_dict_;
    ZSTD_CCtx* compressor_ctx_;
    ZSTD_DCtx* decompressor_ctx_;

    bool use_dictionary_;
};


#endif //CAMBADA_RTDB2COMPRESSORZSTD_H

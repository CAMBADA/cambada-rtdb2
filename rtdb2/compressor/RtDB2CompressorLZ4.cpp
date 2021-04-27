#include "RtDB2CompressorLZ4.h"

#include <lz4.h>
#include <stdexcept>

#include "../RtDB2ErrorCode.h"

RtDB2CompressorLZ4::RtDB2CompressorLZ4() {
}

int RtDB2CompressorLZ4::compress(const std::string &data, std::string& compressed) {
    int dst_size = LZ4_compressBound(data.size());
    if (dst_size == 0)
        return RTDB2_FAILED_COMPRESSING;
    if (dst_size >= RTDB2_COMPRESSOR_BUFFER_SIZE)
        return RTDB2_FAILED_COMPRESSING;

    char buffer[RTDB2_COMPRESSOR_BUFFER_SIZE] = {0};
    int size = LZ4_compress_default(data.c_str(), &buffer[0], data.size(), dst_size);
    compressed.assign(std::string(buffer, size));
    return RTDB2_SUCCESS;
}

int RtDB2CompressorLZ4::decompress(const std::string &data, std::string& decompressed) {
    unsigned long buffer_size;
    unsigned int iteration = 1, factor;
    int size_final;
    do {
        factor = iteration * iteration;

        buffer_size = data.size() * 10 * factor;
        if (buffer_size >= RTDB2_COMPRESSOR_BUFFER_SIZE)
            return RTDB2_FAILED_DECOMPRESSING;
        char buffer[RTDB2_COMPRESSOR_BUFFER_SIZE];
        size_final = LZ4_decompress_safe(data.c_str(), &buffer[0], data.size(), buffer_size);

        if (size_final > 0) {
            decompressed.assign(buffer, size_final);
            return RTDB2_SUCCESS;
        }
        iteration += 1;
    } while (size_final < 0 || iteration > 10);

    return RTDB2_FAILED_DECOMPRESSING;
}

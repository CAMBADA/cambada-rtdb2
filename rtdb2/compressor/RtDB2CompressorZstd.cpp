#include "RtDB2CompressorZstd.h"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include "../RtDB2ErrorCode.h"

RtDB2CompressorZstd::RtDB2CompressorZstd() :
        compressor_dict_(NULL), decompressor_dict_(NULL),
        compressor_ctx_(NULL), decompressor_ctx_(NULL), use_dictionary_(false)
{
}

RtDB2CompressorZstd::RtDB2CompressorZstd(const std::string &dictionary_location) :
        compressor_dict_(NULL), decompressor_dict_(NULL),
        compressor_ctx_(NULL), decompressor_ctx_(NULL), use_dictionary_(false)
{
    use_dictionary(dictionary_location);
}

void RtDB2CompressorZstd::use_dictionary(const std::string& dictionary_location) {
    use_dictionary_ = false;
    std::ifstream file_dict(dictionary_location.c_str(), std::ios::in | std::ios::binary);
    if (!file_dict) {
        std::stringstream ss;
        ss << "Failed to load dictionary at " << dictionary_location;
        throw std::runtime_error(ss.str());
    }
    // Discover the length of the file
    file_dict.seekg(0, file_dict.end);
    long int file_length = file_dict.tellg();
    file_dict.seekg(0, file_dict.beg);

    // Allocate the space and read it
    char * buffer = new char[file_length];
    file_dict.read(buffer, file_length);
    file_dict.close();

    // Create the respective objects for each dictionary
    compressor_dict_ = ZSTD_createCDict(buffer, file_length, 1);
    decompressor_dict_ = ZSTD_createDDict(buffer, file_length);
    delete [] buffer;

    if (compressor_dict_ == NULL || decompressor_dict_ == NULL)
        return;

    // Create the contexts for the decompressor and the compressor
    compressor_ctx_ = ZSTD_createCCtx();
    decompressor_ctx_ = ZSTD_createDCtx();

    if (compressor_ctx_ == NULL || decompressor_ctx_ == NULL)
        return;

    // Use dictionary if both of them have been loaded with success
    use_dictionary_ = true;
}

RtDB2CompressorZstd::~RtDB2CompressorZstd() {
    ZSTD_freeCCtx(compressor_ctx_);
    ZSTD_freeDCtx(decompressor_ctx_);

    ZSTD_freeCDict(compressor_dict_);
    ZSTD_freeDDict(decompressor_dict_);
}

int RtDB2CompressorZstd::compress(const std::string &data, std::string& compressed) {
    size_t buffer_size = ZSTD_compressBound(data.size());
    if (buffer_size >= RTDB2_COMPRESSOR_BUFFER_SIZE)
        return RTDB2_FAILED_COMPRESSING;
    char buffer[RTDB2_COMPRESSOR_BUFFER_SIZE] = {0};

    size_t final_size;
    if (use_dictionary_) {
        final_size = ZSTD_compress_usingCDict(compressor_ctx_, buffer, buffer_size, data.c_str(),
                                              data.size(), compressor_dict_);
    } else {
        final_size = ZSTD_compress(buffer, buffer_size, data.c_str(), data.size(), 1);
    }

    if (ZSTD_isError(final_size)) {
        //printf("Failed to compress data using ZSTD:\n\t%s\n", ZSTD_getErrorName(final_size));
        return RTDB2_FAILED_COMPRESSING;
    }

    compressed.assign(buffer, final_size);
    return RTDB2_SUCCESS;
}

int RtDB2CompressorZstd::decompress(const std::string &compressed_data, std::string& decompressed) {
    unsigned long long decompressed_size = ZSTD_getFrameContentSize(compressed_data.c_str(), compressed_data.size());
    if (decompressed_size == ZSTD_CONTENTSIZE_ERROR || decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        //printf("Failed to detect the size of the data to be decompressed.\n");
        return RTDB2_FAILED_DECOMPRESSING;
    }
    if (decompressed_size >= RTDB2_COMPRESSOR_BUFFER_SIZE)
        return RTDB2_FAILED_DECOMPRESSING;

    char buffer[RTDB2_COMPRESSOR_BUFFER_SIZE] = {0};
    size_t final_size;
    if (use_dictionary_) {
        final_size = ZSTD_decompress_usingDDict(decompressor_ctx_, buffer, decompressed_size,
                                                compressed_data.c_str(), compressed_data.size(),
                                                decompressor_dict_);
    } else {
        final_size = ZSTD_decompress(buffer, decompressed_size,
                                     compressed_data.c_str(), compressed_data.size());
    }

    if (final_size != decompressed_size) {
        //printf("Failed to decompress data:\n\t%s\n", ZSTD_getErrorName(final_size));
        return RTDB2_FAILED_DECOMPRESSING;
    }
    decompressed.assign(buffer, final_size);
    return RTDB2_SUCCESS;
}


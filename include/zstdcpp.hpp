#ifndef ZSTD_CPP_ZSTDCPP_H
#define ZSTD_CPP_ZSTDCPP_H
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <zstd.h>


class zstd {
private:
  std::vector<uint8_t> buffer_;

public:
  template <class T>
  std::vector<uint8_t> compress(const T &data, int compress_level) {
    buffer_.clear();
    auto const bound = ZSTD_compressBound(data.size());

    buffer_.resize(bound);

    auto bytes_per_value =
        sizeof(typename T::value_type);
    auto compressed_size = ZSTD_compress(
        reinterpret_cast<void *>(buffer_.data()), bound, data.data(),
        data.size() * bytes_per_value, compress_level);

    buffer_.resize(compressed_size);
    buffer_.shrink_to_fit();

    return buffer_;
  }
  template <class T>
  std::vector<uint8_t> decompress(const T &data, size_t limit = -1) {
    buffer_.clear();
    auto bytes_per_value =
        sizeof(typename T::value_type);
    auto const estimated =
        ZSTD_getFrameContentSize(data.data(), data.size() * bytes_per_value);

    if (estimated == ZSTD_CONTENTSIZE_UNKNOWN ||
        estimated == ZSTD_CONTENTSIZE_ERROR)
      throw std::overflow_error(
          "Decompressed size unknown, or error occurred when estimating.");

    if (estimated > limit)
      throw std::overflow_error(
           std::string("Estimated decompressed size larger than limit. "));

    buffer_.resize(estimated);

    size_t const decompressed_size =
        ZSTD_decompress(reinterpret_cast<void *>(buffer_.data()), estimated,
                        data.data(), data.size());

    buffer_.resize(decompressed_size);
    buffer_.shrink_to_fit();
    return buffer_;
  }
};

#endif // ZSTD_CPP_ZSTDCPP_H
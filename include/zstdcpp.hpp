#ifndef ZSTD_CPP_ZSTDCPP_H
#define ZSTD_CPP_ZSTDCPP_H
#include <cstdint>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <vector>
#include <zstd.h>

class zstd {
private:
  std::vector<uint8_t> buffer_;

public:
  bool isZstdCompressed(uint8_t *data, int size) {
    const int headerSize = 4; // 根据Zstd的规范，头部大小为4个字节

    // 检查数据是否足够长，以包含Zstd的头部
    if (size < headerSize) {
      return false;
    }

    // 检查数据的头部是否匹配Zstd的标识符
    const uint32_t zstdMagicNumber = 0xFD2FB528;
    uint32_t fileMagicNumber = *reinterpret_cast<const uint32_t *>(data);

    return fileMagicNumber == zstdMagicNumber;
  }

  template <class T>
  std::vector<uint8_t> compress(const T &data, int compress_level) {
    buffer_.clear();
    auto const bound = ZSTD_compressBound(data.size());

    buffer_.resize(bound);

    auto bytes_per_value = sizeof(typename T::value_type);
    auto compressed_size = ZSTD_compress(
        reinterpret_cast<void *>(buffer_.data()), bound, data.data(),
        data.size() * bytes_per_value, compress_level);

    buffer_.resize(compressed_size);
    buffer_.shrink_to_fit();

    return buffer_;
  }
  template <class T>
  std::vector<uint8_t> decompress(const T &data, size_t limit = -1) {
    constexpr auto bytes_per_value = sizeof(typename T::value_type);
    auto begin = (uint8_t *)data.data();
    auto bytes = data.size() * bytes_per_value;
    bool compressed = isZstdCompressed(begin, bytes);
    if (!compressed) {
      auto end = begin + bytes;
      return std::vector<uint8_t>(begin, end);
    }
    buffer_.clear();

    auto const estimated = ZSTD_getFrameContentSize(begin, bytes);

    if (estimated == ZSTD_CONTENTSIZE_UNKNOWN ||
        estimated == ZSTD_CONTENTSIZE_ERROR)
      throw std::overflow_error(
          "Decompressed size unknown, or error occurred when estimating.");

    if (estimated > limit)
      throw std::overflow_error(
          std::string("Estimated decompressed size larger than limit. "));

    buffer_.resize(estimated);

    size_t const decompressed_size = ZSTD_decompress(
        reinterpret_cast<void *>(buffer_.data()), estimated, begin, bytes);

    buffer_.resize(decompressed_size);
    buffer_.shrink_to_fit();
    return buffer_;
  }
};

#endif // ZSTD_CPP_ZSTDCPP_H
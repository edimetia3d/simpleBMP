#ifndef SIMPLEBMP_H
#define SIMPLEBMP_H

#include <memory>
#include <string>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// BMP File has many implementations, but the Windows version is the most common
// one.
//  [CN]: http://www.cnblogs.com/xiekeli/archive/2012/05/09/2491191.html
//  [EN]: https://www.fileformat.info/format/bmp/egff.htm

namespace simpleBMP {
#pragma pack(push, 1) // Disable padding to align to 1 byte
struct BitMapFileHeader {
  uint16_t bfType;      // Fixed value,0x4d42
  uint32_t bfSize;      // File size in bytes, usually zero for uncompressed BMP
  uint16_t bfReserved1; // Always zero
  uint16_t bfReserved2; // Always zero
  uint32_t bfOffBits;   // File offset to the start of the pixel data
};
static_assert(sizeof(BitMapFileHeader) == 14,
              "BitMapFileHeader is not 14 bytes");

struct BitMapInfoHeader {
  uint32_t biSize;        // Size of this header in bytes, we only support 40
  int32_t biWidth;        // Image width in pixels
  int32_t biHeight;       // Image height in pixels
  uint16_t biPlanes;      // Always 1
  uint16_t biBitCount;    // Number of bits per pixel, we only support 8 and 24
  uint32_t biCompression; // Compression type, 0 for uncompressed, usually zero
  uint32_t biSizeImage;   // Used only when biCompression is not 0, usually zero
  int32_t biXPelsPerMeter; // Horizontal resolution in pixels per meter
  int32_t biYPelsPerMeter; // Vertical resolution in pixels per meter
  uint32_t biClrUsed;      // Number of colors used
  uint32_t biClrImportant; // Number of important colors
};
static_assert(sizeof(BitMapInfoHeader) == 40,
              "BitMapInfoHeader is not 40 bytes");

struct PaletteItem {
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t reserved;
};
#pragma pack(pop) // reset to default

enum class SimpleError {
  NO_ERROR,
  FILE_NOT_FOUND,
  FILE_NOT_BMP,
  UNSUPPORTED_BMP_FORMAT,
  HEADER_CORRUPTED,
  COLOR_MAP_CORRUPTED,
  PIXEL_DATA_CORRUPTED,
};

class BMPImg {
public:
  BitMapFileHeader file_header;
  BitMapInfoHeader info_header;
  std::vector<PaletteItem> color_palette;
  std::vector<uint8_t> pixel_data;

  template <class Unused = void> SimpleError LoadImage(const std::string &path);

  template <class Unused = void> SimpleError SaveImage(const std::string &path);

  /**
   * Get the pointer to the first pixel at the specified line.
   */
  uint8_t *AtLine(int line) { return At(line, 0); }

  /**
   * Get the pointer to the pixel data at the specified position.
   * Note that the pixel data may contain multiple bytes
   */
  uint8_t *At(int row, int col) {
    assert(row >= 0 && row < info_header.biHeight && col >= 0 &&
           col < info_header.biWidth && "Invalid position");
    return &pixel_data[(row * line_bytes + col * channel)];
  }

  /**
   * Get Channel data and remove all the padding bytes in every scanline.
   * @param channel_index could be 0 for 8bit, {0,1,2} for 24bit
   * @return A vector of channel data, padding bytes are removed.
   */
  std::vector<uint8_t> GetChannel(int channel_index) {
    assert(channel_index >= 0 && channel_index < channel &&
           "Channel_index out of range");
    std::vector<uint8_t> channel_data;
    channel_data.reserve(info_header.biWidth * info_header.biHeight);
    for (int row = 0; row < info_header.biHeight; ++row) {
      auto beg = AtLine(row) + channel_index;
      auto end = beg + info_header.biWidth * channel;
      for (; beg < end; beg += channel) {
        channel_data.push_back(*beg);
      }
    }
    return channel_data;
  }

  int Channel() const { return channel; }

private:
  int64_t line_bytes;
  int channel = 1;
};

template <class> SimpleError BMPImg::LoadImage(const std::string &path) {
  std::shared_ptr<FILE> file(fopen(path.c_str(), "rb"), [](FILE *p) {
    if (p) {
      fclose(p);
    }
  });
  auto p_file = file.get();
  if (!p_file) {
    return SimpleError::FILE_NOT_FOUND;
  }
  // Try read file header
  fread(&file_header.bfType, sizeof(file_header.bfType), 1, p_file);
  if (file_header.bfType != 0x4D42) { // Check is it an RGB file
    return SimpleError::FILE_NOT_BMP;
  }
  // Read remaining
  fread(&file_header.bfSize,
        sizeof(BitMapFileHeader) - sizeof(file_header.bfType), 1, p_file);

  // Read info header
  fread(&info_header.biSize, sizeof(info_header.biSize), 1, p_file);
  if (info_header.biSize != 40) {
    return SimpleError::UNSUPPORTED_BMP_FORMAT;
  }
  // Read remaining
  fread(&info_header.biWidth,
        sizeof(BitMapInfoHeader) - sizeof(info_header.biSize), 1, p_file);
  // Read color map when it is present, only when info_header.biBitCount <= 8.
  if (info_header.biBitCount == 8) {
    color_palette.resize(1 << info_header.biBitCount);
    fread(color_palette.data(), sizeof(PaletteItem), color_palette.size(),
          p_file);
    channel = 1;
  } else if (info_header.biBitCount == 24) {
    channel = 3;
  } else {
    return SimpleError::UNSUPPORTED_BMP_FORMAT;
  }

  // Read pixel data, note that
  // 1. in 24bit bmp, pixel data is stored in BGR order
  // 2. Every row is forced to align to 4 bytes
  int line_pixel = info_header.biWidth * channel;
  line_bytes = (line_pixel + 3) / 4 * 4; // align to 4 bytes
  int64_t byte_count = line_bytes * info_header.biHeight;
  pixel_data.resize(byte_count);
  fread(pixel_data.data(), byte_count, 1, p_file);
  return SimpleError::NO_ERROR;
}

template <class Unused> SimpleError BMPImg::SaveImage(const std::string &path) {
  std::shared_ptr<FILE> file(fopen(path.c_str(), "wb"), [](FILE *p) {
    if (p) {
      fclose(p);
    }
  });
  auto p_file = file.get();
  if (!p_file) {
    return SimpleError::FILE_NOT_FOUND;
  }
  if (file_header.bfType != 0x4D42) {
    assert(false && "File header corrupted");
    return SimpleError::HEADER_CORRUPTED;
  }
  fwrite(&file_header, sizeof(BitMapFileHeader), 1, p_file);
  if (info_header.biSize != 40) {
    assert(false && "Info header corrupted");
    return SimpleError::HEADER_CORRUPTED;
  }
  fwrite(&info_header, sizeof(BitMapInfoHeader), 1, p_file);
  if (channel == 1) {
    if (info_header.biBitCount != 8) {
      assert(false && "Info header corrupted");
      return SimpleError::HEADER_CORRUPTED;
    }
    if (color_palette.size() != (1 << info_header.biBitCount)) {
      assert(false && "Color map corrupted");
      return SimpleError::COLOR_MAP_CORRUPTED;
    }
    fwrite(color_palette.data(), sizeof(PaletteItem), color_palette.size(),
           p_file);
  }
  // recalculate line bytes to make sure it is correct
  int line_pixel = info_header.biWidth * channel;
  line_bytes = (line_pixel + 3) / 4 * 4; // align to 4 bytes
  int64_t byte_count = line_bytes * info_header.biHeight;
  if (pixel_data.size() != byte_count) {
    assert(false && "Pixel data or header may get corrupted");
    return SimpleError::PIXEL_DATA_CORRUPTED;
  }
  fwrite(pixel_data.data(), pixel_data.size(), 1, p_file);
  return SimpleError::NO_ERROR;
}
} // namespace simpleBMP
#endif

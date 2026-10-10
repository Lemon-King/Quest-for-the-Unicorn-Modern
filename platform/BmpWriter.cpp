/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "BmpWriter.h"

#include <cstdint>
#include <cstdio>
#include <vector>

#include "Canvas.h"

namespace platform {

namespace {

void put16(std::vector<uint8_t>& bytes, uint32_t value) {
  bytes.push_back(static_cast<uint8_t>(value & 0xff));
  bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xff));
  }

void put32(std::vector<uint8_t>& bytes, uint32_t value) {
  put16(bytes, value & 0xffff);
  put16(bytes, value >> 16);
  }

}

BmpWriter::BmpWriter(const Canvas& canvas)
  : canvas_(canvas) {
  }

bool BmpWriter::write(const char* filename) const {
  uint32_t width = static_cast<uint32_t>(canvas_.width());
  uint32_t height = static_cast<uint32_t>(canvas_.height());
  uint32_t rowSize = (width * 3 + 3) & ~3u;   /* BMP rows are padded to 4 bytes */
  uint32_t imageSize = rowSize * height;

  std::vector<uint8_t> bytes;
  bytes.reserve(14 + 40 + imageSize);

  /* BITMAPFILEHEADER (14 bytes); the pixels follow both headers */
  bytes.push_back('B');
  bytes.push_back('M');
  put32(bytes, 14 + 40 + imageSize);
  put32(bytes, 0);
  put32(bytes, 14 + 40);

  /* BITMAPINFOHEADER (40 bytes): 24 bits per pixel, uncompressed, 72 dpi */
  put32(bytes, 40);
  put32(bytes, width);
  put32(bytes, height);
  put16(bytes, 1);
  put16(bytes, 24);
  put32(bytes, 0);
  put32(bytes, imageSize);
  put32(bytes, 2835);
  put32(bytes, 2835);
  put32(bytes, 0);
  put32(bytes, 0);

  /* Bottom row first, each pixel as B,G,R */
  const uint32_t* pixels = canvas_.pixels();
  for (uint32_t row = height; row-- > 0;) {
    const uint32_t* line = pixels + static_cast<size_t>(row) * width;
    for (uint32_t column = 0; column < width; column++) {
      bytes.push_back(static_cast<uint8_t>(line[column] & 0xff));
      bytes.push_back(static_cast<uint8_t>((line[column] >> 8) & 0xff));
      bytes.push_back(static_cast<uint8_t>((line[column] >> 16) & 0xff));
      }
    for (uint32_t padding = width * 3; padding < rowSize; padding++) bytes.push_back(0);
    }

  std::FILE* file = std::fopen(filename, "wb");
  if (file == nullptr) return false;
  size_t written = std::fwrite(bytes.data(), 1, bytes.size(), file);
  bool   closed = std::fclose(file) == 0;
  return closed && written == bytes.size();
  }

}

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "IconWriter.h"

#include <fstream>
#include <stdexcept>

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

/* No pixel is scaled or invented, so the sprite must fit as it is */
IconWriter::IconWriter(const platform::Image& sprite, int32_t size) : sprite_(sprite), size_(size) {
  if (size < 1 || size > 255 || size < sprite.width() || size < sprite.height())
    throw std::invalid_argument("the sprite does not fit the icon size");
  }

/* Each sprite pixel at ((size - width)/2 + x, (size - height)/2 + y); opaque ones fully opaque, the rest clear */
std::vector<uint32_t> IconWriter::pixels() const {
  std::vector<uint32_t> square(static_cast<size_t>(size_) * static_cast<size_t>(size_), 0);
  int32_t               left = (size_ - sprite_.width()) / 2;
  int32_t               top = (size_ - sprite_.height()) / 2;

  for (int32_t y = 0; y < sprite_.height(); y++)
    for (int32_t x = 0; x < sprite_.width(); x++)
      if (sprite_.opaque(x, y))
        square[static_cast<size_t>(top + y) * static_cast<size_t>(size_) + static_cast<size_t>(left + x)] =
          0xff000000u | sprite_.pixel(x, y);
  return square;
  }

/* ICONDIR, one ICONDIRENTRY, then the image: BITMAPINFOHEADER (height doubled for the mask), 32-bit pixels bottom row
   first, and the 1-bit AND mask, rows padded to 32 bits, a set bit marking a transparent pixel */
void IconWriter::write(const std::filesystem::path& file) const {
  std::vector<uint32_t> square = pixels();
  uint32_t              size = static_cast<uint32_t>(size_);
  uint32_t              maskRow = ((size + 31) / 32) * 4;
  uint32_t              imageBytes = size * size * 4 + maskRow * size;
  std::vector<uint8_t>  bytes;

  put16(bytes, 0);
  put16(bytes, 1);
  put16(bytes, 1);

  bytes.push_back(static_cast<uint8_t>(size));
  bytes.push_back(static_cast<uint8_t>(size));
  bytes.push_back(0);
  bytes.push_back(0);
  put16(bytes, 1);
  put16(bytes, 32);
  put32(bytes, 40 + imageBytes);
  put32(bytes, 6 + 16);

  put32(bytes, 40);
  put32(bytes, size);
  put32(bytes, size * 2);
  put16(bytes, 1);
  put16(bytes, 32);
  put32(bytes, 0);
  put32(bytes, imageBytes);
  put32(bytes, 0);
  put32(bytes, 0);
  put32(bytes, 0);
  put32(bytes, 0);

  for (uint32_t row = size; row-- > 0;)
    for (uint32_t x = 0; x < size; x++) put32(bytes, square[row * size + x]);

  for (uint32_t row = size; row-- > 0;) {
    std::vector<uint8_t> mask(maskRow, 0);
    for (uint32_t x = 0; x < size; x++)
      if ((square[row * size + x] >> 24) == 0) mask[x / 8] |= static_cast<uint8_t>(0x80 >> (x % 8));
    bytes.insert(bytes.end(), mask.begin(), mask.end());
    }

  std::ofstream out(file, std::ios::binary | std::ios::trunc);
  out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
  if (!out) throw std::runtime_error("could not write " + file.string());
  }

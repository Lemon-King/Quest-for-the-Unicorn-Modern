/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "XpmDecoder.h"

#include <array>
#include <stdexcept>
#include <string>
#include <utility>

namespace platform {

namespace {

bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
  }

const char* skipSpace(const char* text) {
  while (*text != 0 && isSpace(*text)) text++;
  return text;
  }

int32_t readNumber(const char*& text) {
  text = skipSpace(text);
  if (*text < '0' || *text > '9') throw std::invalid_argument("XPM header is not four numbers");
  int32_t value = 0;
  while (*text >= '0' && *text <= '9') {
    if (value > 100000) throw std::invalid_argument("XPM header number too large");
    value = value * 10 + (*text - '0');
    text++;
    }
  return value;
  }

uint32_t hexDigit(char c) {
  if (c >= '0' && c <= '9') return static_cast<uint32_t>(c - '0');
  if (c >= 'a' && c <= 'f') return static_cast<uint32_t>(c - 'a' + 10);
  if (c >= 'A' && c <= 'F') return static_cast<uint32_t>(c - 'A' + 10);
  throw std::invalid_argument("XPM colour has a bad hex digit");
  }

/* One 16-bit channel of four hex digits, scaled to 8 bits as graphics.c did: above 255 it is divided by 256 */
uint32_t channel(const char* digits) {
  uint32_t value = 0;
  for (int i = 0; i < 4; i++) value = value * 16 + hexDigit(digits[i]);
  return (value > 255) ? value / 256 : value;
  }

}

Image XpmDecoder::decode(const char* const* xpm) const {
  if (xpm == nullptr || xpm[0] == nullptr) throw std::invalid_argument("no XPM data");

  const char* header = xpm[0];
  int32_t width = readNumber(header);
  int32_t height = readNumber(header);
  int32_t colours = readNumber(header);
  int32_t perPixel = readNumber(header);
  if (width < 1 || height < 1 || colours < 1) throw std::invalid_argument("XPM size is empty");
  if (perPixel != 1) throw std::invalid_argument("only one character per pixel is supported");

  std::array<bool, 256>     known{};
  std::array<bool, 256>     transparent{};
  std::array<uint32_t, 256> palette{};

  /* "<key> c <colour>", read like graphics.c's sscanf("%c %c %s"): the key may itself be a space */
  for (int32_t i = 0; i < colours; i++) {
    const char* line = xpm[1 + i];
    if (line == nullptr || line[0] == 0) throw std::invalid_argument("XPM colour line missing");

    uint8_t     key = static_cast<uint8_t>(line[0]);
    const char* rest = skipSpace(line + 1);
    if (*rest != 'c') throw std::invalid_argument("XPM colour line is not a \"c\" colour");
    rest = skipSpace(rest + 1);

    std::string token;
    while (*rest != 0 && !isSpace(*rest)) token += *rest++;

    known[key] = true;
    if (token == "None") transparent[key] = true;
    else if (token.size() == 13 && token[0] == '#')
      palette[key] = (channel(token.c_str() + 1) << 16) | (channel(token.c_str() + 5) << 8) | channel(token.c_str() + 9);
    else throw std::invalid_argument("unsupported XPM colour \"" + token + "\"");
    }

  size_t                count = static_cast<size_t>(width) * static_cast<size_t>(height);
  std::vector<uint32_t> pixels(count, 0);
  std::vector<bool>     opaque(count, false);

  for (int32_t y = 0; y < height; y++) {
    const char* row = xpm[1 + colours + y];
    if (row == nullptr) throw std::invalid_argument("XPM pixel row missing");

    for (int32_t x = 0; x < width; x++) {
      if (row[x] == 0) throw std::invalid_argument("XPM pixel row too short");
      uint8_t key = static_cast<uint8_t>(row[x]);
      if (!known[key]) throw std::invalid_argument("XPM pixel uses an undefined colour");

      size_t at = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
      pixels[at] = palette[key];
      opaque[at] = !transparent[key];
      }
    }

  return Image(width, height, std::move(pixels), std::move(opaque));
  }

}

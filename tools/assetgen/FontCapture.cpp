/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "FontCapture.h"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

/* TextOutA draws at (8,16) on its baseline, so a glyph's 8x12 cell covers x 8-15 and y 6-17 of the surface */
constexpr int SURFACE = 32;
constexpr int ORIGIN_X = 8;
constexpr int BASELINE = 16;
constexpr int CELL_WIDTH = 8;
constexpr int CELL_HEIGHT = 12;
constexpr int CELL_ASCENT = 10;
constexpr int CELL_TOP = BASELINE - CELL_ASCENT;

std::string hexByte(uint32_t value) {
  static const char digits[] = "0123456789abcdef";
  std::string text = "0x";
  text += digits[(value >> 4) & 0xf];
  text += digits[value & 0xf];
  return text;
  }

}

FontCapture::FontCapture()
  : dc_(nullptr), section_(nullptr), previousBitmap_(nullptr), bits_(nullptr), font_(nullptr), previousFont_(nullptr),
    metrics_{}, rows_{} {
  /* As rcs_initWin32 did, so Windows hands out the font without DPI virtualisation */
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  UINT codePage = GetOEMCP();
  if (codePage != 437) throw std::runtime_error("the OEM code page is " + std::to_string(codePage) + ", not 437");

  try {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = SURFACE;
    info.bmiHeader.biHeight = -SURFACE;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    dc_ = CreateCompatibleDC(nullptr);
    if (dc_ == nullptr) throw std::runtime_error("CreateCompatibleDC failed");
    section_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (section_ == nullptr || bits == nullptr) throw std::runtime_error("CreateDIBSection failed");
    bits_ = static_cast<uint32_t*>(bits);
    previousBitmap_ = SelectObject(dc_, section_);

    /* The exact call graphics.c made */
    font_ = CreateFontA(12, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Terminal");
    if (font_ == nullptr) throw std::runtime_error("CreateFontA(\"Terminal\") failed");
    previousFont_ = SelectObject(dc_, font_);

    if (!GetTextMetricsA(dc_, &metrics_)) throw std::runtime_error("GetTextMetricsA failed");
    if (metrics_.tmHeight != CELL_HEIGHT || metrics_.tmAscent != CELL_ASCENT || metrics_.tmMaxCharWidth != CELL_WIDTH)
      throw std::runtime_error("the Terminal font is " + std::to_string(metrics_.tmMaxCharWidth) + "x" +
                               std::to_string(metrics_.tmHeight) + " with ascent " + std::to_string(metrics_.tmAscent) +
                               ", not 8x12 with ascent 10");

    capture();
    }
  catch (...) {
    release();
    throw;
    }
  }

FontCapture::~FontCapture() {
  release();
  }

void FontCapture::release() {
  if (dc_ != nullptr) {
    if (previousFont_ != nullptr) SelectObject(dc_, previousFont_);
    if (previousBitmap_ != nullptr) SelectObject(dc_, previousBitmap_);
    }
  if (font_ != nullptr) DeleteObject(font_);
  if (section_ != nullptr) DeleteObject(section_);
  if (dc_ != nullptr) DeleteDC(dc_);
  dc_ = nullptr;
  section_ = nullptr;
  font_ = nullptr;
  previousBitmap_ = nullptr;
  previousFont_ = nullptr;
  bits_ = nullptr;
  }

/* Byte 0 stays blank: a C string cannot contain it */
void FontCapture::capture() {
  SetTextAlign(dc_, TA_LEFT | TA_BASELINE);
  SetBkMode(dc_, TRANSPARENT);
  SetTextColor(dc_, RGB(255, 255, 255));

  for (uint32_t code = 1; code < 256; code++) {
    GdiFlush();
    std::memset(bits_, 0, sizeof(uint32_t) * SURFACE * SURFACE);

    char character = static_cast<char>(code);
    TextOutA(dc_, ORIGIN_X, BASELINE, &character, 1);
    GdiFlush();

    for (int y = 0; y < SURFACE; y++)
      for (int x = 0; x < SURFACE; x++) {
        uint32_t pixel = bits_[y * SURFACE + x] & 0x00ffffff;
        if (pixel == 0) continue;
        if (pixel != 0x00ffffff) throw std::runtime_error("glyph " + hexByte(code) + " has a pixel neither lit nor clear");
        if (x < ORIGIN_X || x >= ORIGIN_X + CELL_WIDTH || y < CELL_TOP || y >= CELL_TOP + CELL_HEIGHT)
          throw std::runtime_error("glyph " + hexByte(code) + " draws outside its 8x12 cell");
        rows_[code][static_cast<size_t>(y - CELL_TOP)] |= static_cast<uint8_t>(0x80 >> (x - ORIGIN_X));
        }
    }
  }

void FontCapture::writeHeader(const std::filesystem::path& file) const {
  std::ofstream out(file);
  if (!out) throw std::runtime_error("cannot write " + file.string());

  out << "/*\n"
         " * The Windows \"Terminal\" raster font (OEM_CHARSET, 8x12) as the GDI build drew it with TextOutA, one glyph"
         " per byte.\n"
         " * Generated by tools/assetgen (\"assetgen font\") on a system with OEM code page 437; do not edit.\n"
         " */\n"
         "\n"
         "#pragma once\n"
         "\n"
         "#include <cstdint>\n"
         "\n"
         "namespace platform {\n"
         "\n"
         "constexpr int32_t TERMINAL_FONT_HEIGHT = " << metrics_.tmHeight << ";\n"
         "constexpr int32_t TERMINAL_FONT_ASCENT = " << metrics_.tmAscent << ";\n"
         "constexpr int32_t TERMINAL_FONT_ADVANCE = " << CELL_WIDTH << ";\n"
         "\n"
         "/* One byte per row, leftmost pixel in the top bit */\n"
         "constexpr uint8_t TERMINAL_FONT_ROWS[256][TERMINAL_FONT_HEIGHT] = {\n";

  for (uint32_t code = 0; code < 256; code++) {
    out << "  { ";
    for (size_t row = 0; row < CELL_HEIGHT; row++) out << (row != 0 ? "," : "") << hexByte(rows_[code][row]);
    out << " },   /* " << hexByte(code) << " */\n";
    }

  out << "  };\n"
         "\n"
         "}\n";
  out.close();
  if (!out) throw std::runtime_error("cannot write " + file.string());
  }

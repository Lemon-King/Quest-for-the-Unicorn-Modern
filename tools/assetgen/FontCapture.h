/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#pragma once

#include <windows.h>

#include <array>
#include <cstdint>
#include <filesystem>

/* Draws every byte with the Terminal font the way the GDI build did, and records the lit pixels of each glyph */
class FontCapture {
  public:
    FontCapture();
    ~FontCapture();

    FontCapture(const FontCapture&) = delete;
    FontCapture& operator=(const FontCapture&) = delete;

    void writeHeader(const std::filesystem::path& file) const;

  private:
    void capture();
    void release();

    HDC                                    dc_;
    HBITMAP                                  section_;
    HGDIOBJ                                  previousBitmap_;
    uint32_t*                                bits_;
    HFONT                                    font_;
    HGDIOBJ                                  previousFont_;
    TEXTMETRICA                              metrics_;
    std::array<std::array<uint8_t, 12>, 256> rows_;
  };

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

#include <cstdint>

namespace platform {

class Canvas;

/* A fixed-pitch bitmap font: 256 glyphs, 8 pixels wide, one byte per row */
class GlyphFont {
  public:
    GlyphFont(const uint8_t (*rows)[12], int32_t height, int32_t ascent, int32_t advance);

    void draw(Canvas& canvas, int32_t x, int32_t baseline, const char* text, uint32_t rgb) const;

  private:
    const uint8_t (*rows_)[12];
    int32_t       height_;
    int32_t       ascent_;
    int32_t       advance_;
  };

}

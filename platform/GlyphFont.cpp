/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "GlyphFont.h"

#include <stdexcept>

#include "Canvas.h"

namespace platform {

GlyphFont::GlyphFont(const uint8_t (*rows)[12], int32_t height, int32_t ascent, int32_t advance)
  : rows_(rows), height_(height), ascent_(ascent), advance_(advance) {
  if (rows == nullptr) throw std::invalid_argument("a font needs its glyph rows");
  if (height < 1 || height > 12) throw std::invalid_argument("glyph height must be 1 to 12");
  if (ascent < 0 || ascent > height) throw std::invalid_argument("glyph ascent must lie within its height");
  if (advance < 1) throw std::invalid_argument("glyph advance must be positive");
  }

/* Where graphics.c put each TextOut cell: one character per 8-pixel column, on the baseline, transparent background */
void GlyphFont::draw(Canvas& canvas, int32_t x, int32_t baseline, const char* text, uint32_t rgb) const {
  for (int32_t i = 0; text[i] != 0; i++)
    canvas.mask(x + i * advance_, baseline - ascent_, rows_[static_cast<uint8_t>(text[i])], height_, rgb);
  }

}

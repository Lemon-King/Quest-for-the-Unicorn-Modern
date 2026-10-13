/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The canvas, font and sprite part of platform.h. No exception crosses into C: drawing calls that fail do nothing,
   and the others return 0 or NULL */

#include "platform.h"

#include <exception>

#include "ApiTypes.h"
#include "BmpWriter.h"
#include "TerminalFont.h"

extern "C" CANVAS* canvas_create(int32_t width, int32_t height) {
  try {
    return new CANVAS{platform::Canvas(width, height)};
    }
  catch (const std::exception&) {
    return nullptr;
    }
  }

extern "C" void canvas_destroy(CANVAS* canvas) {
  delete canvas;
  }

extern "C" void canvas_fill(CANVAS* canvas, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb) {
  canvas->canvas.fill(x, y, width, height, rgb);
  }

extern "C" void canvas_line(CANVAS* canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb) {
  canvas->canvas.line(x1, y1, x2, y2, rgb);
  }

extern "C" void canvas_point(CANVAS* canvas, int32_t x, int32_t y, uint32_t rgb) {
  canvas->canvas.point(x, y, rgb);
  }

extern "C" void canvas_text(CANVAS* canvas, const GLYPH_FONT* font, int32_t x, int32_t baseline, const char* text,
                            uint32_t rgb) {
  font->font.draw(canvas->canvas, x, baseline, text, rgb);
  }

extern "C" void canvas_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y, const char* const* xpm) {
  try {
    canvas->canvas.image(x, y, sprites->cache.sprite(xpm));
    }
  catch (const std::exception&) {
    }
  }

extern "C" void canvas_half_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y,
                                   const char* const* xpm) {
  try {
    canvas->canvas.image(x, y, sprites->cache.halfSprite(xpm));
    }
  catch (const std::exception&) {
    }
  }

extern "C" void canvas_copy(CANVAS* target, const CANVAS* source) {
  try {
    target->canvas.copyFrom(source->canvas);
    }
  catch (const std::exception&) {
    }
  }

extern "C" int32_t canvas_save_bmp(const CANVAS* canvas, const char* filename) {
  try {
    return platform::BmpWriter(canvas->canvas).write(filename) ? 1 : 0;
    }
  catch (const std::exception&) {
    return 0;
    }
  }

extern "C" const GLYPH_FONT* terminal_font(void) {
  static const GLYPH_FONT terminal{platform::GlyphFont(platform::TERMINAL_FONT_ROWS, platform::TERMINAL_FONT_HEIGHT,
                                                       platform::TERMINAL_FONT_ASCENT, platform::TERMINAL_FONT_ADVANCE)};
  return &terminal;
  }

extern "C" SPRITE_CACHE* sprite_cache_create(void) {
  try {
    return new SPRITE_CACHE{};
    }
  catch (const std::exception&) {
    return nullptr;
    }
  }

extern "C" void sprite_cache_destroy(SPRITE_CACHE* sprites) {
  delete sprites;
  }

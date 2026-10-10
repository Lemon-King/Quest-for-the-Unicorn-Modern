/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 * The Windows layer as the C game sees it. platform.lib implements it in C++; header.h cannot be included from C++
 * because it uses "class" as a field name, so everything crosses through this header.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CANVAS       CANVAS;
typedef struct GLYPH_FONT   GLYPH_FONT;
typedef struct SPRITE_CACHE SPRITE_CACHE;

/* A back buffer of 0x00RRGGBB pixels. Drawing outside it is clipped. */
CANVAS*           canvas_create(int32_t width, int32_t height);
void              canvas_destroy(CANVAS* canvas);
void              canvas_fill(CANVAS* canvas, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb);
void              canvas_line(CANVAS* canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb);
void              canvas_point(CANVAS* canvas, int32_t x, int32_t y, uint32_t rgb);
void              canvas_text(CANVAS* canvas, const GLYPH_FONT* font, int32_t x, int32_t baseline, const char* text, uint32_t rgb);
void              canvas_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y, const char* const* xpm);
void              canvas_copy(CANVAS* target, const CANVAS* source);
int32_t           canvas_save_bmp(const CANVAS* canvas, const char* filename);
const uint32_t*   canvas_pixels(const CANVAS* canvas);
int32_t           canvas_take_dirty(CANVAS* canvas, int32_t* x, int32_t* y, int32_t* width, int32_t* height);

/* The Terminal font the GDI build drew all its text with */
const GLYPH_FONT* terminal_font(void);

/* Sprites decoded once from the XPM arrays in unicorn.xpm */
SPRITE_CACHE*     sprite_cache_create(void);
void              sprite_cache_destroy(SPRITE_CACHE* sprites);

#ifdef __cplusplus
  }
#endif

#endif

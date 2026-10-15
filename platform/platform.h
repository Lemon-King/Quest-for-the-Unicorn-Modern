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
typedef struct GAME_HOST    GAME_HOST;

/* A back buffer of 0x00RRGGBB pixels. Drawing outside it is clipped. A tiled sprite fills its rectangle with the
   sprite repeated from originX,originY. */
CANVAS*           canvas_create(int32_t width, int32_t height);
void              canvas_destroy(CANVAS* canvas);
void              canvas_fill(CANVAS* canvas, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb);
void              canvas_line(CANVAS* canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb);
void              canvas_point(CANVAS* canvas, int32_t x, int32_t y, uint32_t rgb);
void              canvas_text(CANVAS* canvas, const GLYPH_FONT* font, int32_t x, int32_t baseline, const char* text, uint32_t rgb);
void              canvas_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y, const char* const* xpm);
void              canvas_half_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y, const char* const* xpm);
void              canvas_tiled_sprite(CANVAS* canvas, SPRITE_CACHE* sprites, int32_t x, int32_t y, int32_t width,
                                      int32_t height, int32_t originX, int32_t originY, const char* const* xpm);
void              canvas_copy(CANVAS* target, const CANVAS* source);
int32_t           canvas_save_bmp(const CANVAS* canvas, const char* filename);

/* The Terminal font the GDI build drew all its text with */
const GLYPH_FONT* terminal_font(void);

/* Sprites decoded once from the XPM arrays in unicorn.xpm */
SPRITE_CACHE*     sprite_cache_create(void);
void              sprite_cache_destroy(SPRITE_CACHE* sprites);

/* What configure_game() tells the host about the window to open */
typedef struct {
  int32_t width;
  int32_t height;
  int32_t gameThread;       /* 1: run_game() on its own thread, the window on the main thread */
  int32_t integerScaling;   /* 1: whole-number scales only, black borders */
  } GAME_SETUP;

/* The game as the host starts it: configure_game() applies unicorn.cfg and the command line, reads the maps and
   returns 0 when no window should open (the -s report); run_game() plays until the player saves and leaves */
int32_t           configure_game(int32_t argc, char** argv, GAME_SETUP* setup);
void              run_game(GAME_HOST* host);

/* Answers of host_peek_key() and host_read_key() that are not keys */
#define HOST_KEY_NONE    (-2)
#define HOST_KEY_CLOSED  (-1)

/* What the game asks of its host */
void              host_present(GAME_HOST* host, CANVAS* canvas);
int32_t           host_peek_key(GAME_HOST* host);
int32_t           host_read_key(GAME_HOST* host);
void              host_sleep(GAME_HOST* host, int32_t milliseconds);
int32_t           host_closed(GAME_HOST* host);

/* Tells the player why the game has to close, in a message box; the caller then ends the game */
void              show_fatal_error(const char* message);

#ifdef __cplusplus
  }
#endif

#endif

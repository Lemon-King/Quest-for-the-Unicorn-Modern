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
 * Borland BGI stroked fonts (.CHR files).
 *
 * The copyright text ends with 0x1A, followed by the offset of the stroke header. That header holds the glyph count,
 * the first character, the offset of the stroke data and the cap height, then a word offset and a byte width for each
 * glyph. A glyph is a list of byte pairs: 7-bit signed x and y in font units, with y pointing up from the baseline.
 * The top bits of the pair select end (00), move (10) or draw (11).
 */

#include "header.h"

#define CHR_SIGNATURE    "PK\x08\x08"
#define STROKE_SIGNATURE '+'
#define MAX_SIZE         10

struct STROKE_FONT {
  const byte* data;
  Int32       length;
  const byte* offsets;
  const byte* widths;
  const byte* strokes;
  Int32       count;
  Int32       first;
  Int32       cap;
  };

/* Size 4 draws the font as designed; the Graph unit scales other sizes by mul/div. */
static const Int32 SIZE_MUL[MAX_SIZE + 1] = { 0, 3, 2, 3, 1, 4, 5, 2, 5, 3, 4 };
static const Int32 SIZE_DIV[MAX_SIZE + 1] = { 0, 5, 3, 4, 1, 3, 3, 1, 2, 1, 1 };

static Int32 read_word(const byte* at) {
  return at[0] | (at[1] << 8);
  }

static Int32 font_unit(byte value) {
  Int32 unit;
  unit = value & 0x7f;
  return (unit & 0x40) ? unit - 128 : unit;
  }

STROKE_FONT* stroke_font_create(const byte* data,Int32 length) {
  STROKE_FONT* font;
  const byte*  end_of_text;
  Int32        header;
  Int32        count;
  Int32        stroke_offset;

  if (length < 4 || memcmp(data,CHR_SIGNATURE,4) != 0) return NULL;

  end_of_text = memchr(data,0x1a,length);
  if (end_of_text == NULL || end_of_text + 3 > data + length) return NULL;

  header = read_word(end_of_text + 1);
  if (header + 16 > length || data[header] != STROKE_SIGNATURE) return NULL;

  count = read_word(data + header + 1);
  stroke_offset = read_word(data + header + 5);
  if (count <= 0 || header + 16 + 3 * count > length || header + stroke_offset >= length) return NULL;

  font = malloc(sizeof(STROKE_FONT));
  if (font == NULL) return NULL;

  font->data = data;
  font->length = length;
  font->count = count;
  font->first = data[header + 4];
  font->cap = (signed char)data[header + 8];
  font->offsets = data + header + 16;
  font->widths = font->offsets + 2 * count;
  font->strokes = data + header + stroke_offset;

  return font;
  }

static void draw_glyph(const STROKE_FONT* font,Int32 glyph,Int32 x,Int32 baseline,Int32 pen,Int32 size,Int32 color) {
  const byte* stroke;
  const byte* end;
  Int32       op;
  Int32       px,py;
  Int32       last_x,last_y;

  stroke = font->strokes + read_word(font->offsets + 2 * glyph);
  end = font->data + font->length - 1;
  last_x = x;
  last_y = baseline;

  while (stroke < end) {
    op = ((stroke[0] >> 7) << 1) | (stroke[1] >> 7);
    if (op == 0) return;

    px = x + (pen + font_unit(stroke[0])) * SIZE_MUL[size] / SIZE_DIV[size];
    py = baseline - font_unit(stroke[1]) * SIZE_MUL[size] / SIZE_DIV[size];

    /* BGI lines include both end points; plotting the end keeps that on every backend. */
    if (op == 3) {
      draw(last_x,last_y,px,py,color);
      plot(px,py,color);
      }

    last_x = px;
    last_y = py;
    stroke += 2;
    }
  }

/* The glyph for letter, or -1 when the font has none */
static Int32 find_glyph(const STROKE_FONT* font,char letter) {
  Int32 glyph;

  glyph = (byte)letter - font->first;
  if (glyph < 0 || glyph >= font->count) return -1;

  return glyph;
  }

void stroke_font_draw(const STROKE_FONT* font,Int32 x,Int32 y,Int32 size,Int32 color,char* text) {
  Int32 glyph;
  Int32 baseline;
  Int32 pen;

  if (size < 1 || size > MAX_SIZE) return;

  baseline = y + font->cap * SIZE_MUL[size] / SIZE_DIV[size];
  pen = 0;

  for (; *text != 0; text++) {
    glyph = find_glyph(font,*text);
    if (glyph < 0) continue;

    draw_glyph(font,glyph,x,baseline,pen,size,color);
    pen += font->widths[glyph];
    }
  }

Int32 stroke_font_width(const STROKE_FONT* font,Int32 size,char* text) {
  Int32 glyph;
  Int32 pen;

  if (size < 1 || size > MAX_SIZE) return 0;

  pen = 0;

  for (; *text != 0; text++) {
    glyph = find_glyph(font,*text);
    if (glyph < 0) continue;

    pen += font->widths[glyph];
    }

  return pen * SIZE_MUL[size] / SIZE_DIV[size];
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Scroll text, each line centred on display_scroll()'s parchment, in write_font()'s hand-drawn letters or, with the
   Gothic Scrolls preference, in Gothic */

#include "header.h"

#define PARCHMENT_TOP     50
#define PARCHMENT_HEIGHT 380
#define CENTER_X         250
#define MARKER_X         226     /* centres the 48-pixel "more ^" */
#define TOP_ROD_Y         34
#define BOTTOM_ROD_Y     455

#define GOTHIC_SIZE        3     /* display_scroll()'s settextstyle(4,0,3) */
#define RUNE_DROP          5     /* puts write_font()'s baseline on Gothic's */

#define MAX_LINES         64
#define MAX_LINE_LENGTH  100

typedef char SCROLL_LINE[MAX_LINE_LENGTH];

/* gothic is NULL for the hand-drawn letters; ink_offset centres a line's letters in its line_height */
typedef struct {
  STROKE_FONT* gothic;
  Int32        text_width;
  Int32        line_height;
  Int32        lines_shown;
  Int32        ink_offset;
  } SCROLL_STYLE;

/* 30 letters to a line, as scrolls always set them; the letters ink from 2 to 13 below a line's y */
static const SCROLL_STYLE LETTER_STYLE = {
  .gothic = NULL, .text_width = 270, .line_height = 19, .lines_shown = 18, .ink_offset = 1 };

/* Gothic size 3 inks from 3 above a line's y to 20 below */
static const SCROLL_STYLE GOTHIC_STYLE = {
  .gothic = NULL, .text_width = 360, .line_height = 25, .lines_shown = 14, .ink_offset = 4 };

/* Unread letters are raised by 128 and drawn as runes */
static char is_rune(char letter) {
  return ((byte)letter >= 128) ? 'Y' : 'N';
  }

/* Copies the run of runes, or of other letters, that text starts with */
static Int32 copy_run(char* text,char* run) {
  Int32 length;
  char  rune;

  rune = is_rune(text[0]);
  length = 0;

  while (text[length] != 0 && is_rune(text[length]) == rune) {
    run[length] = text[length];
    length++;
    }
  run[length] = 0;

  return length;
  }

/* Runes keep the hand-drawn letters in Gothic too */
static char in_gothic(const SCROLL_STYLE* style,char* run) {
  return (style->gothic != NULL && is_rune(run[0]) == 'N') ? 'Y' : 'N';
  }

static Int32 run_width(const SCROLL_STYLE* style,char* run) {
  if (in_gothic(style,run) == 'Y') return stroke_font_width(style->gothic,GOTHIC_SIZE,run);

  return write_font_width(run);
  }

static void draw_run(const SCROLL_STYLE* style,char* run,Int32 x,Int32 y) {
  if (in_gothic(style,run) == 'Y') {
    stroke_font_draw(style->gothic,x,y,GOTHIC_SIZE,0,run);
    return;
    }

  if (style->gothic != NULL) y += RUNE_DROP;
  write_font(x,y,0,run);
  }

static Int32 line_width(const SCROLL_STYLE* style,char* text) {
  char  run[MAX_LINE_LENGTH];
  Int32 width;

  width = 0;

  while (*text != 0) {
    text += copy_run(text,run);
    width += run_width(style,run);
    }

  return width;
  }

static void draw_line(const SCROLL_STYLE* style,char* text,Int32 y) {
  char  run[MAX_LINE_LENGTH];
  Int32 x;

  x = CENTER_X - line_width(style,text) / 2;

  while (*text != 0) {
    text += copy_run(text,run);
    draw_run(style,run,x,y);
    x += run_width(style,run);
    }
  }

/* Copies the word text starts with, up to a space or line break */
static Int32 copy_word(char* text,char* word) {
  Int32 length;

  length = 0;

  while (text[length] != 0 && text[length] != ' ' && text[length] != '\n' && length < MAX_LINE_LENGTH - 1) {
    word[length] = text[length];
    length++;
    }
  word[length] = 0;

  return length;
  }

/* Breaks message into lines that fit the parchment, at spaces and at each '\n' */
static Int32 wrap_lines(const SCROLL_STYLE* style,char* message,SCROLL_LINE* lines) {
  char  word[MAX_LINE_LENGTH];
  char  joined[2 * MAX_LINE_LENGTH];
  Int32 current;
  char  fits;

  current = 0;
  strcpy(lines[current],"");

  while (*message != 0) {
    if (*message == ' ') {
      message++;
      }
    else if (*message == '\n') {
      if (current == MAX_LINES - 1) break;

      current++;
      strcpy(lines[current],"");
      message++;
      }
    else {
      message += copy_word(message,word);

      strcpy(joined,lines[current]);
      if (strlen(joined) > 0) strcat(joined," ");
      strcat(joined,word);

      fits = (strlen(joined) < MAX_LINE_LENGTH && line_width(style,joined) <= style->text_width) ? 'Y' : 'N';

      if (strlen(lines[current]) == 0 || fits == 'Y') {
        strcpy(lines[current],joined);
        }
      else {
        if (current == MAX_LINES - 1) break;

        current++;
        strcpy(lines[current],word);
        }
      }
    }

  return current + 1;
  }

/* The lines from top that fit */
static void draw_page(const SCROLL_STYLE* style,SCROLL_LINE* lines,Int32 count,Int32 top) {
  Int32 i;
  Int32 rows;
  Int32 first_y;

  rows = (count < style->lines_shown) ? count : style->lines_shown;
  first_y = PARCHMENT_TOP + (PARCHMENT_HEIGHT - rows * style->line_height) / 2 + style->ink_offset;

  display_scroll();

  for (i=0; i<rows; i++) draw_line(style,lines[top + i],first_y + i * style->line_height);

  setcolor(0);
  if (top > 0) outtextxy(MARKER_X,TOP_ROD_Y,"more ^");
  if (top + rows < count) outtextxy(MARKER_X,BOTTOM_ROD_Y,"more v");
  }

static Int32 clamp_top(Int32 top,Int32 last_top) {
  if (top < 0) return 0;
  if (top > last_top) return last_top;

  return top;
  }

/* Shows message on the scroll and waits for a key; 8/2 and 9/3 scroll text too long to fit */
void scroll_text(char* message) {
  SCROLL_LINE  lines[MAX_LINES];
  SCROLL_STYLE style;
  STROKE_FONT* gothic;
  Int32        count;
  Int32        page;
  Int32        top;
  Int32        last_top;
  char         key;
  char         done;

  gothic = (gothic_scrolls == 'Y') ? gothic_font() : NULL;
  style = (gothic != NULL) ? GOTHIC_STYLE : LETTER_STYLE;
  style.gothic = gothic;

  count = wrap_lines(&style,message,lines);
  page = style.lines_shown - 1;
  last_top = (count > style.lines_shown) ? count - style.lines_shown : 0;
  top = 0;

  done = 'N';
  while (done == 'N') {
    draw_page(&style,lines,count,top);
    key = inkey();

    if (key == '8') top = clamp_top(top - 1,last_top);
    if (key == '2') top = clamp_top(top + 1,last_top);
    if (key == '9') top = clamp_top(top - page,last_top);
    if (key == '3') top = clamp_top(top + page,last_top);

    if (last_top == 0) done = 'Y';
    if (key != '8' && key != '2' && key != '9' && key != '3') done = 'Y';
    }
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The compass, and the name that the name view mode and Floor Item Names show. They sit on the strip along the
   screen's bottom, right of the status icons. When Icons mode gives that strip to the map, they sit on the detail
   line at the foot of the log's box, the compass at half size, and the Tab Menu covers them with the log. */

#include "header.h"

#define COMPASS_X     300     /* past the twelve status icons */
#define NAME_LEFT     325     /* past the twelve status icons and the compass */
#define NAME_START    160     /* where a name starts, from the screen's right edge */
#define NAME_Y         15     /* the baseline, from the screen's bottom edge */
#define DETAIL_LEFT   215     /* the log's left edge, from the screen's right edge */
#define DETAIL_NAME    16     /* where the detail line's name starts, past the compass */
#define CELL_WIDTH      8
#define MAX_NAME      127
#define NO_COMPASS     10     /* the black tile */

static Int32 compass_sprite = NO_COMPASS;
static char  name[MAX_NAME+1];
static Int32 name_color = 0;
static Int32 name_length = 0;

/* A name too long to start in its place on the strip moves left, leaving one blank cell at the screen's edge */
static Int32 strip_name_x(Int32 length) {
  Int32 x;

  x = screen_width - NAME_START;
  if (x + CELL_WIDTH * (length + 1) > screen_width) x = screen_width - CELL_WIDTH * (length + 1);

  return x;
  }

static Int32 detail_compass_x(void) {
  return screen_width - DETAIL_LEFT;
  }

static Int32 detail_name_x(void) {
  return detail_compass_x() + DETAIL_NAME;
  }

static Int32 longest_name(void) {
  Int32 longest;

  if (has_bottom_strip()) longest = (screen_width - NAME_LEFT) / CELL_WIDTH - 1;
    else longest = (screen_width - detail_name_x()) / CELL_WIDTH - 1;
  if (longest > MAX_NAME) longest = MAX_NAME;

  return longest;
  }

/* Text over the name's cells, in the current colour */
static void draw_name(char* text) {
  char shown[MAX_NAME+1];

  sprintf(shown, "%.*s", longest_name(), text);
  if (has_bottom_strip()) outtextxyclr(strip_name_x(name_length), screen_height - NAME_Y, shown);
    else outtextxyclr(detail_name_x(), detail_line_top() + 11, shown);
  }

static void draw_detail_compass(void) {
  setfillstyle(1,0);
  bar(detail_compass_x(),detail_line_top(),detail_compass_x() + half_tile_width(),detail_line_bottom());
  if (compass_sprite != NO_COMPASS) put_half_image(detail_compass_x(),detail_line_top(),select_sprite(compass_sprite));
  }

/* Clears the name, if one is showing */
void hide_bottom_name(void) {
  char blank[MAX_NAME+1];

  if (name_length == 0) return;

  sprintf(blank, "%*s", name_length, "");
  draw_name(blank);
  name_length = 0;
  }

/* Shows text in color without its trailing spaces */
void show_bottom_name(char* text, Int32 color) {
  Int32 length;

  hide_bottom_name();

  length = (Int32)strlen(text);
  while (length > 0 && text[length-1] == ' ') length--;
  if (length > longest_name()) length = longest_name();
  if (length <= 0) return;

  strncpy(name, text, length);
  name[length] = 0;
  name_color = color;
  name_length = length;

  setcolor(color);
  draw_name(name);
  }

/* The compass's arrow sprite, or NO_COMPASS for none */
void show_compass(Int32 sprite) {
  compass_sprite = sprite;

  if (has_bottom_strip()) {
    putimage(COMPASS_X,screen_height-tile_height,select_sprite(NO_COMPASS),0);
    putimage(COMPASS_X,screen_height-tile_height,select_sprite(sprite),0);
    }
  else draw_detail_compass();
  }

/* Each mode shows its own compass, on a screen drawn afresh */
void forget_compass(void) {
  compass_sprite = NO_COMPASS;
  }

/* The compass and the name again, on the detail line when there is one */
void show_detail_line(void) {
  if (has_bottom_strip()) return;

  draw_detail_compass();
  if (name_length > 0) {
    setcolor(name_color);
    draw_name(name);
    }
  }

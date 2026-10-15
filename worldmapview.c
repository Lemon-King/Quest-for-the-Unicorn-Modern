/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The world map, which M opens on the overworld with the World Map preference: every region side by side, as large as
   fits, each square the same square shape, showing only the squares you have found, in the minimap's colours or in
   each tile's art. Markers show you, your docked ship and the towns, dungeons, villages and Gateway you have found; the
   direction keys move between them, and the one selected is named under its marker. */

#include "header.h"

#define HEADING          "WORLD MAP"
#define HEADING_SIZE      4      /* Gothic at the title screen's size for "Quest" */
#define HEADING_TOP       6
#define MAP_TOP          40      /* below the heading */
#define MAP_SIDES         4
#define MAP_FOOT         22      /* below the map, for the key tips */
#define TIPS             "(8/2/4/6) Select   (ESC) Back"
#define TIPS_RAISE        8      /* the key tips' baseline, above the screen's bottom edge */
#define LABEL_HEIGHT     14      /* write_font()'s letters, their tails included */
#define LABEL_MARGIN      2

/* The overworld's squares as width by height pixels from left,top */
typedef struct {
  Int32 left;
  Int32 top;
  Int32 width;
  Int32 height;
  } WORLD_VIEW;

/* Paints squares first to last of overworld column px, which share value */
typedef void (*PAINT_SQUARES)(const WORLD_VIEW* view,Int32 px,Int32 first,Int32 last,Int32 value);

/* As large as fits between the heading and the key tips, every square the same shape, centred */
static WORLD_VIEW fit_view(void) {
  WORLD_VIEW view;
  Int32      room_width,room_height;

  room_width = screen_width - 2 * MAP_SIDES;
  room_height = screen_height - MAP_FOOT - MAP_TOP;
  if (room_width * WORLD_HEIGHT <= room_height * WORLD_WIDTH) {
    view.width = room_width;
    view.height = room_width * WORLD_HEIGHT / WORLD_WIDTH;
    }
  else {
    view.height = room_height;
    view.width = room_height * WORLD_WIDTH / WORLD_HEIGHT;
    }
  view.left = (screen_width - view.width) / 2;
  view.top = MAP_TOP + (room_height - view.height) / 2;

  return view;
  }

/* The screen column where overworld column px starts; column WORLD_WIDTH + 1 starts at the map's right edge */
static Int32 column_left(const WORLD_VIEW* view,Int32 px) {
  return view->left + (px - 1) * view->width / WORLD_WIDTH;
  }

static Int32 row_top(const WORLD_VIEW* view,Int32 py) {
  return view->top + (py - 1) * view->height / WORLD_HEIGHT;
  }

/* Paints each run of one value down a column with a single call */
static void paint_column(const WORLD_VIEW* view,Int32 px,const Int32* column,PAINT_SQUARES paint) {
  Int32 first;
  Int32 py;

  first = 1;
  for (py=2; py<=WORLD_HEIGHT + 1; py++)
    if (py > WORLD_HEIGHT || column[py] != column[first]) {
      paint(view,px,first,py - 1,column[first]);
      first = py;
      }
  }

/* Squares first to last of column px in a minimap colour; black is the screen's own */
static void fill_squares(const WORLD_VIEW* view,Int32 px,Int32 first,Int32 last,Int32 color) {
  if (color == 0) return;

  setfillstyle(1,color);
  bar(column_left(view,px),row_top(view,first),column_left(view,px + 1),row_top(view,last + 1));
  }

/* Each square you have found in its minimap colour. A tile without one takes the colour of the found square drawn
   before it, down each column in turn, as 7.0's minimap did while you explored. */
static void draw_colors(const WORLD_VIEW* view) {
  Int32 column[WORLD_HEIGHT + 1];
  Int32 carried;
  Int32 px,py;

  carried = 0;
  for (px=1; px<=WORLD_WIDTH; px++) {
    for (py=1; py<=WORLD_HEIGHT; py++) {
      column[py] = 0;
      if (!world_known(px,py)) continue;

      column[py] = land_color(world_tile(px,py));
      if (column[py] == 0) column[py] = carried;
      carried = column[py];
      }
    paint_column(view,px,column,fill_squares);
    }
  }

/* Squares first to last of column px in their tile's art, which repeats from the map's corner */
static void tile_squares(const WORLD_VIEW* view,Int32 px,Int32 first,Int32 last,Int32 tile) {
  if (tile == 0) return;

  put_tiled_image(column_left(view,px),row_top(view,first),column_left(view,px + 1),row_top(view,last + 1),
                  view->left,view->top,select_sprite(tile));
  }

/* Each square you have found shows its tile's art at full size, so a stretch of one kind of land reads as that land's
   art, cut to its shape */
static void draw_art(const WORLD_VIEW* view) {
  Int32 column[WORLD_HEIGHT + 1];
  Int32 px,py;

  for (px=1; px<=WORLD_WIDTH; px++) {
    for (py=1; py<=WORLD_HEIGHT; py++) column[py] = world_known(px,py) ? world_tile(px,py) : 0;
    paint_column(view,px,column,tile_squares);
    }
  }

/* The top left of a place's marker: its sprite at half size, centred on its square */
static void marker_corner(const WORLD_VIEW* view,const WORLD_PLACE* place,Int32* x,Int32* y) {
  *x = (column_left(view,place->px) + column_left(view,place->px + 1) - half_tile_width()) / 2;
  *y = (row_top(view,place->py) + row_top(view,place->py + 1) - half_tile_height()) / 2;
  }

/* Yours last, over any other */
static void draw_markers(const WORLD_VIEW* view,const WORLD_PLACES* places) {
  Int32 x,y;
  Int32 i;

  for (i=places->count - 1; i>=0; i--) {
    marker_corner(view,&places->list[i],&x,&y);
    put_half_image(x,y,select_sprite(places->list[i].sprite));
    }
  }

/* The heading in Gothic, as the title screen writes, or on the text grid without the font */
static void draw_heading(void) {
  STROKE_FONT* font;

  font = gothic_font();
  if (font == NULL) {
    write_centred(1,CLR_YELLOW,HEADING);
    return;
    }

  stroke_font_draw(font,(screen_width - stroke_font_width(font,HEADING_SIZE,HEADING)) / 2,HEADING_TOP,HEADING_SIZE,
                   CLR_YELLOW,HEADING);
  }

static void draw_tips(void) {
  setcolor(CLR_CYAN);
  outtextxyclr((screen_width - 8 * (Int32)strlen(TIPS)) / 2,screen_height - TIPS_RAISE,TIPS);
  }

/* The place's name in yellow scroll letters on black, under its marker, or over it where the map ends, kept on the
   screen */
static void draw_label(const WORLD_VIEW* view,const WORLD_PLACE* place) {
  char  name[sizeof(place->name)];
  Int32 marker_x,marker_y;
  Int32 x,y;
  Int32 width;

  strcpy(name,place->name);
  width = write_font_width(name);
  marker_corner(view,place,&marker_x,&marker_y);

  x = marker_x + (half_tile_width() - width) / 2;
  if (x > screen_width - LABEL_MARGIN - width) x = screen_width - LABEL_MARGIN - width;
  if (x < LABEL_MARGIN) x = LABEL_MARGIN;

  y = marker_y + half_tile_height() + 1;
  if (y + LABEL_HEIGHT > view->top + view->height) y = marker_y - LABEL_HEIGHT - 1;

  setfillstyle(1,CLR_BLACK);
  bar(x - LABEL_MARGIN,y,x + width + LABEL_MARGIN,y + LABEL_HEIGHT);
  write_font(x,y,CLR_YELLOW,name);
  }

/* The map is drawn once and kept; each key brings it back and names the place selected. A direction key moves the
   selection; ESC, ENTER and M close the map. */
void show_world_map(void) {
  WORLD_PLACES places;
  WORLD_VIEW   view;
  Int32        selected;
  Int32        d;
  char         key;

  find_world_places(&places);
  view = fit_view();

  graphmode();
  if (world_map == 'A') draw_art(&view);
    else draw_colors(&view);
  draw_markers(&view,&places);
  draw_heading();
  draw_tips();
  saveScreen();

  selected = 0;
  key = 0;
  while (key != 27 && key != 13 && key != 'M') {
    restoreScreen();
    draw_label(&view,&places.list[selected]);
    key = inkey();

    d = key_step(key);
    if (d >= 0) selected = next_world_place(&places,selected,step_x(d),step_y(d));
    }
  }

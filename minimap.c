/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The minimap beside the map view: MINIMAP_SQUARES squares a side, two pixels each. It shows the map you are on, or on
   the overworld with the Seamless World Map preference On the part of the world around you, kept inside the world. */

#include "header.h"

/* The overworld position at the corner the minimap was last drawn from */
static Int32 drawn_px;
static Int32 drawn_py;

static Int32 follows_player(void) {
  return (seamless_world() && seamless_world_map == 'Y');
  }

/* The overworld position of the minimap's top left square, centred on the player as far as the world allows */
static Int32 corner(Int32 player_at,Int32 first,Int32 world_size) {
  Int32 at;

  if (!follows_player()) return first;

  at = player_at - MINIMAP_SQUARES / 2;
  if (at < 1) at = 1;
  if (at > world_size - MINIMAP_SQUARES + 1) at = world_size - MINIMAP_SQUARES + 1;

  return at;
  }

static Int32 corner_px(void) {
  return corner(getPX(player.region,player.x),getPX(player.region,1),WORLD_WIDTH);
  }

static Int32 corner_py(void) {
  return corner(getPY(player.region,player.y),getPY(player.region,1),WORLD_HEIGHT);
  }

/* The map square in the minimap's top left corner */
Int32 minimap_left(void) {
  if (!follows_player()) return 1;

  return corner_px() - getPX(player.region,0);
  }

Int32 minimap_top(void) {
  if (!follows_player()) return 1;

  return corner_py() - getPY(player.region,0);
  }

/* Plots map square x,y in color. On the joined overworld a square outside the minimap is left out. */
void plot_minimap(Int32 x,Int32 y,Int32 color) {
  Int32 column,row;

  column = x - minimap_left();
  row = y - minimap_top();
  if (seamless_world() &&
      (column < 0 || column >= MINIMAP_SQUARES || row < 0 || row >= MINIMAP_SQUARES)) return;

  plot(smap_offset_x+column*2,smap_offset_y+1+row*2,color);
  plot(smap_offset_x+1+column*2,smap_offset_y+1+row*2,color);
  plot(smap_offset_x+column*2,smap_offset_y+2+row*2,color);
  plot(smap_offset_x+1+column*2,smap_offset_y+2+row*2,color);
  }

/* An overworld tile's colour, or 0 for one without a colour of its own */
Int32 land_color(Int32 tile) {
  switch (tile) {
    case  1:return 10;
    case  2:return 2;
    case  3:return 6;
    case  4:return 1;
    case  5:return 4;
    case  6:return 3;
    case  7:return 8;
    case  8:return 14;
    case  9:return 5;
    case 11:return 4;
    case 17:return 4;
    case 19:return 6;
    }

  return 0;
  }

/* Whether the minimap now shows another part of the overworld than when show_sm_land() last drew it */
Int32 minimap_moved(void) {
  return (corner_px() != drawn_px || corner_py() != drawn_py);
  }

void minimap_drawn(void) {
  drawn_px = corner_px();
  drawn_py = corner_py();
  }

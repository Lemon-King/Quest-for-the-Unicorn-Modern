/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "header.h"

/* An overworld square in the view, its tile or darkness */
void show_land_square(Int32 x,Int32 y) {
  if (in_sight(x,y) != '*') return;

  if (square_known(x,y))
    show_sprite(x-player.map_x,y-player.map_y,square_value(x,y));
  else
    show_sprite(x-player.map_x,y-player.map_y,10);
  }

void show_land() {
  Int32 x,y;
  show_map_frames();
  setfillstyle(1,0);
  bar(10,10,TILES_X * tile_width+10,TILES_Y * tile_height+10);
  for (x=player.map_x; x<=player.map_x+TILES_X; x++)
    for (y=player.map_y; y<=player.map_y+TILES_Y; y++)
      show_land_square(x,y);
  show_headings();
  }

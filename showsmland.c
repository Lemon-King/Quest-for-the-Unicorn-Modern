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

void show_sm_land() {
  Int32 x,y;
  Int32 left,top;
  Int32 color;
  pixel = 0;
  setfillstyle(1,0);
  bar(smap_offset_x,smap_offset_y,smap_offset_x+99,smap_offset_y+99);
  left = minimap_left();
  top = minimap_top();
  for (x=left; x<left+MINIMAP_SQUARES; x++)
    for (y=top; y<top+MINIMAP_SQUARES; y++) {
      color = square_known(x,y) ? land_color(square_value(x,y)) : 0;
      plot_minimap(x,y,color);
      }
  minimap_drawn();
  close_display();
  }

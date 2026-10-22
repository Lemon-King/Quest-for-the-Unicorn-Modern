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

void remove_land_player() {
  Int32 tile;
  tile = square_value(player.x,player.y);
  show_sprite(player.x-player.map_x,player.y-player.map_y,tile);
  plot_minimap(player.x,player.y,land_color(tile));
  close_display();
  }

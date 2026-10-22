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

void show_land_player() {
  if (player.inship == ' ')
    show_sprite(player.x-player.map_x,player.y-player.map_y,rider_sprite());
  else
    show_sprite(player.x-player.map_x,player.y-player.map_y,12);
  plot_minimap(player.x,player.y,15);
  close_display();
  }

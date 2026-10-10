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

void teleport() {
  player.x = u_random(100) + 1;
  player.y = u_random(100) + 1;
  while (player.map[player.x][player.y] != currentFloor) {
    player.x = u_random(100) + 1;
    player.y = u_random(100) + 1;
    }
  player.map_x = player.x - (TILES_X / 2);
  player.map_y = player.y - (TILES_Y / 2);
  if (player.map_x < 1) player.map_x = 1;
  if (player.map_y < 1) player.map_y = 1;
  if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
  if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
  last_target_x = 0;
  last_target_y = 0;
  }


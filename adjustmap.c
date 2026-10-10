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

char adjust_map() {
  char changed;
  changed = 'N';
  if (player.map_x > 100-TILES_X) {
    player.map_x = 100-TILES_X;
    changed = 'Y';
    }
  if (player.map_y > 100-TILES_Y) {
    player.map_y = 100-TILES_Y;
    changed = 'Y';
    }
  if (player.map_x < 0) {
    player.map_x = 0;
    changed = 'Y';
    }
  if (player.map_y < 0) {
    player.map_y = 0;
    changed = 'Y';
    }
  if (player.y - player.map_y < 2) {
    player.map_y = player.y - (TILES_Y-4);
    changed = 'Y';
    if (player.map_y < 0) player.map_y = 0;
    }
  if (player.y - player.map_y > TILES_Y-2) {
    player.map_y = player.y - 4;
    changed = 'Y';
    if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
    }
  if (player.x - player.map_x < 2) {
    player.map_x = player.x - (TILES_X-4);
    changed = 'Y';
    if (player.map_x < 0) player.map_x = 0;
    }
  if (player.x - player.map_x > TILES_X-2) {
    player.map_x = player.x - 4;
    changed = 'Y';
    if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
    }
  return changed;
  }


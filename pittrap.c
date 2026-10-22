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

void pit_trap() {
  Int32 i,j;
  Int32 x,y;
  if (player.d_level >= bottom) return;
  save_level();
  if (current_dungeon.level[player.d_level-1].cleared == 0) {
    if (check_level_cleared(player.d_level-1)) {
      current_dungeon.level[player.d_level-1].cleared = -1;
      stats.counts[STAT_LEVELS_CLEARED]++;
      j = -1;
      for (i=0; i<10; i++) if (current_dungeon.level[i].cleared == 0) j = 0;
      if (j) stats.counts[STAT_DUNGEON_CLEARED]++;
      }
    }
  player.d_level++;
  make_map('D');
  levelCleared = check_level_cleared(player.d_level-1);
  player.x = 10 + u_random(80);
  player.y = 10 + u_random(80);
  while (player.map[player.x][player.y] != dungeon_floor) {
    player.x = 10 + u_random(80);
    player.y = 10 + u_random(80);
    }
  player.map_x = player.x - TILES_X/2;
  player.map_y = player.y - TILES_Y/2;
  if (player.map_x < 1) player.map_x = 1;
  if (player.map_y < 1) player.map_y = 1;
  if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
  if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
  camera_arrive();
/*      activeDelay = 2; */
  if (in_room(player.x,player.y) && can_see()) light_room();
  log_pit_trap();
  if (player.party[0].hp > 0) {
    x = player.party[0].treasure & 0xff;
    y = (player.party[0].treasure >> 8) & 0xff;
    player.map[x][y] = player.party[0].prev;
    if (find_empty(player.x,player.y,10,dungeon_floor,&x,&y)) {
      player.party[0].prev = player.map[x][y];
      player.map[x][y] = 300;
      player.party[0].treasure = (y << 8) | x;
      }
    else {
      player.party[0].hp = 0;
      }
    }
  show_screen();
  run_x = 0;
  run_y = 0;
  }


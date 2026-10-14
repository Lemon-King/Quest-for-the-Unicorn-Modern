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

void go_up() {
  char flag;
  if (player.d_level > 0) {
    if (player.d_level > 0 && player.in_city != 'Y') save_level();
    flag = ' ';
    player.d_level--;

    /* Level 0 leaves the dungeon, as the stairs do; without the Words of Power change it loads level[-1] */
    if (change_words_of_power == 'Y' && player.d_level == 0) return;
    make_map(' ');
    if (flag == ' ') {
      player.x = u_random(80)+10; player.y = u_random(80) + 10;
      while (player.map[player.x][player.y] != currentFloor) {
        player.x = u_random(80)+10; player.y = u_random(80) + 10;
        }
      }
    player.map_x = player.x - 8;
    player.map_y = player.y - 8;
    adjust_map();
/*
    if (player.map_x < 1) player.map_x = 1;
    if (player.map_y < 1) player.map_y = 1;
    if (player.map_x > 84) player.map_x = 84;
    if (player.map_y > 84) player.map_y = 84;
*/
    camera_arrive();
    show_screen();
    }
  }


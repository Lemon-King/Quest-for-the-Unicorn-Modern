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

void move_snake(Int32 x,Int32 y,Int32 floor) {
  Int32 xdir,ydir;
  Int32 targetx;
  Int32 targety;
  Int32 i;
  char dir;
  NPC_REC* target;

  dir = '5';
  xdir = 0;
  ydir = 0;
  if (find_target(x,y,&targetx,&targety)) {
    dir = find_path(x,y,targetx,targety,'N');
    if (dir != '5') {
      switch (dir) {
        case '1': xdir = -1; ydir = +1; break;
        case '2': ydir = +1; break;
        case '3': xdir = +1; ydir = +1; break;
        case '4': xdir = -1; break;
        case '6': xdir = +1; break;
        case '7': xdir = -1; ydir = -1; break;
        case '8': ydir = -1; break;
        case '9': xdir = +1; ydir = -1; break;
        }
      }
    }
  if (player.map[x+xdir][y+ydir] < 0) {
    target = &(player.monster[1][0-player.map[x+xdir][y+ydir]]);
    i = get_skill(SK_MAGIC);
    noise(1000,100);
    log_hit_shout();
    if (in_sight(x+xdir,y+ydir) == '*') {
      show_sprite(x+xdir-player.map_x,y+ydir-player.map_y,100);
      delay(delay_hit);
      show_spot(x+xdir,y+ydir);
      }
    target->hp -= i;
    log_snake_bite(target,i);
    dead_monster(target,'Y',x+xdir,y+ydir);
    player.map[x][y] = dungeon_floor;
    show_spot(x,y);
    }
  else if (player.map[x+xdir][y+ydir] == floor) {
    player.map[x][y] = floor;
    show_spot(x,y);
    player.map[x+xdir][y+ydir] = 143;
    show_spot(x+xdir,y+ydir);
    }
  }

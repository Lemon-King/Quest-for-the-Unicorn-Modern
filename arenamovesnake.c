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

static char valid(Int32 floor,Int32 x,Int32 y) {
  char flag;
  flag = '*';
  if (player.map[x][y] != floor) flag = ' ';
  if (x == player.x && y == player.y) flag = ' ';
  if (player.map[x][y]>399 && player.map[x][y]<500) flag = '*';
  return flag;
  }

static void snake_target(Int32 x,Int32 y,Int32* targetx,Int32* targety,Int32* dist) {
  Int32 x1,y1;
  *dist = 32767;
  *targetx = player.x;
  *targety = player.y;
  for (x1=offset_x; x1<=offset_x+16; x1++)
    for (y1=offset_y; y1<=offset_y+16; y1++)
      if (player.map[x1][y1]>399 && player.map[x1][y1]<500 &&
          (sqr(x1-x)+sqr(y1-y)) < *dist) {
        *targetx = x1;
        *targety = y1;
        *dist = (sqr(x1-x)+sqr(y1-y));
        }
  }

void arena_move_snake(Int32 x,Int32 y,Int32 floor) {
  Int32 xdir,ydir;
  Int32 ox,oy;
  Int32 targetx;
  Int32 targety;
  Int32 dist;
  Int32 i;
  Int32 target;

  snake_target(x,y,&targetx,&targety,&dist);
  xdir = 0;
  ydir = 0;
  ox = x; oy = y;
  if (x < targetx && y < targety) { xdir = 1; ydir = 1; }
  if (x > targetx && y < targety) { xdir = -1; ydir = 1; }
  if (x < targetx && y > targety) { xdir = 1; ydir = -1; }
  if (x > targetx && y > targety) { xdir = -1; ydir = -1; }
  if (x == targetx && y < targety) ydir = 1;
  if (x == targetx && y > targety) ydir = -1;
  if (x < targetx && y == targety) xdir = 1;
  if (x > targetx && y == targety) xdir = -1;
  if (dist == 32767) { xdir = 0; ydir = 0; }
  if (valid(floor,x+xdir,y+ydir) == '*') { x+=xdir; y+=ydir; }
  else if (valid(floor,x+xdir,y) == '*') x+=xdir;
  else if (valid(floor,x,y+ydir) == '*') y += ydir;
  player.map[ox][oy] = floor;
  if (in_sight(ox,oy) == '*') show_spot(ox,oy);
  if (player.map[x][y] == floor) {
    player.map[x][y] = 143;
    if (in_sight(x,y) == '*') show_spot(x,y);
    player.map[x][y] = 144;
    }
  if (player.map[x][y]>399 && player.map[x][y]<500) {
    snakes--;
    target = player.map[x][y] - 400;
    i = get_skill(SK_MAGIC);
    noise(1000,100);
    log_hit_shout();
    if (in_sight(x,y) == '*') {
      show_sprite(x-player.map_x,y-player.map_y,100);
      delay(delay_hit);
      show_spot(x,y);
      }
    player.monster[target][a_mon].hp -= i;
    log_snake_bite(&(player.monster[target][a_mon]),i);
    dead_monster(&(player.monster[target][a_mon]),'Y',x,y);
    }
  }

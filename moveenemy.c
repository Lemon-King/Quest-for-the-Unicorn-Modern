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

static char valid(Int32 monster,Int32 x,Int32 y) {
  Int32 i;
  char flag;
  flag = '*';
  if (x<1 || y<1 || x>99 || y>99) flag = ' ';
  if (flag == '*') {
    if (player.map[x][y] < 0) flag = ' ';
    if (player.map[x][y] == 21) flag = ' ';
    if (player.map[x][y] == 22 && u_random(100) < 75)
      player.map[x][y] = 23;
    else if (player.map[x][y] == 22) flag = ' ';
    if (player.map[x][y] == 33 && u_random(100) < 10)
      player.map[x][y] = 23;
    else if (player.map[x][y] == 33) flag = ' ';
    if (x == player.x && y == player.y) flag = ' ';
    if (x<2 || y<2 || x>98 || y>98) flag = ' ';
    if (player.map[x][y] == 4) flag = ' ';
    if (player.map[x][y] == 41) flag = ' ';
    if (player.map[x][y] == 38) flag = ' ';
    if (player.map[x][y] == 39) flag = ' ';
    if (player.map[x][y] == 3) flag = ' ';
    if (player.map[x][y] == 145) flag = ' ';
    if (player.map[x][y] >= 600 && player.map[x][y] < 700) {
      i = player.monster[1][0-monster].level / 4;
      if (i < 1) i = 1;
      player.map[x][y] -= i;
      if (player.map[x][y] < 600) {
        player.map[x][y] = currentFloor;
        }
      else flag = ' ';
      }
    }
  return flag;
  }

void move_enemy(Int32 x,Int32 y) {
  Int32 i;
  Int32 ix,iy;
  Int32 xdir,ydir;
  Int32 ox,oy;
  Int32 monster;
  Int32 spd;
  char fflag;
  char dir;
  Int32 dx,dy;
  Int32 sx,sy;
  Int32 dist;

  monster = player.map[x][y];
  sx = 0;
  sy = 0;
  fflag = ' ';
  dir = '5';
  for (spd=1; spd<=player.monster[1][0-monster].speed; spd++)
    if (fflag != '*') {
      xdir = 0;
      ydir = 0;
      ox = x;
      oy = y;
      if ((player.monster[1][0-monster].special & MS_FRIENDLY) == MS_FRIENDLY ||
          player.monster[1][0-monster].confused > 0) {
        dir = ' ';
        switch (u_random(9)) {
          case 0:xdir = 1; break;
          case 1:xdir = -1; break;
          case 2:ydir = 1; break;
          case 3:ydir = -1; break;
          case 4:xdir = 1; ydir = 1; break;
          case 5:xdir = 1; ydir = -1; break;
          case 6:xdir = -1; ydir = 1; break;
          case 7:xdir = -1; ydir = -1; break;
          case 8:xdir = 0; ydir = 0; break;
          }
        } else {
        if (player.monster[1][0-monster].fear> 0) dir = fear_dir(x,y,player.x,player.y);
        else {
          dx = player.x;
          dy = player.y;
          if (player.party[0].hp > 0) {
            dist = sqr(player.x-x) + sqr(player.y-y);
            sx = player.party[0].treasure & 0xff;
            sy = (player.party[0].treasure >> 8) & 0xff;
            if ((sqr(sx-x)+sqr(sy-y)) < dist) {
              dx = sx;
              dy = sy;
              }
            }
          dir = find_path(x,y,dx,dy,'N');
          if (dir == '5') dir = find_path(x,y,dx,dy,'Y');
          }
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
      if (dir == '5') {
         for (ix=x-1; ix<=x+1; ix++)
           for (iy=y-1; iy<=y+1; iy++) {
             if (player.map[ix][iy] == 22 && u_random(100) < player.monster[1][0-monster].level) {
               player.map[ix][iy] = 23;
               show_spot(ix,iy);
               }
             if (player.map[ix][iy] == 33 && u_random(1000) < player.monster[1][0-monster].level) {
               player.map[ix][iy] = 23;
               show_spot(ix,iy);
               }
             }
         }
      else {
        /* ********************************* */
        /* ***** See if hit the player ***** */
        /* ********************************* */
        if (x+xdir == player.x && y+ydir == player.y &&
            (player.monster[1][0-monster].special & MS_FRIENDLY) != MS_FRIENDLY) {
          enemy_hit(x,y);
          }
        /* ********************************************* */
        /* ***** Else see if hit summoned creature ***** */
        /* ********************************************* */
        else if (x+xdir == sx && y+ydir == sy &&
            (player.monster[1][0-monster].special & MS_FRIENDLY) != MS_FRIENDLY) {
          i = 50 + player.monster[1][0-monster].level * 2;
          if (player.party[0].sleep > 0) i += 50;
          if (player.party[0].fear > 0) i += 25;

          /* Without the Summon Combat change the summon's level and defense are read from party[i], i being the hit
             chance, and the blow does the summon's own attack in damage */
          if (change_summon_combat == 'Y') {
            i -= player.party[0].level * 2;
            i -= player.party[0].defense * 2;
            }
          else {
            i -= player.party[i].level * 2;
            i -= player.party[i].defense * 2;
            }
          if (i < 20) i = 20;
          if (i > 80) i = 80;
          if (u_random(100) < i) {
            if (in_sight(x+xdir,y+ydir) == '*') {
              show_sprite(x+xdir-player.map_x,y+ydir-player.map_y,100);
              delay(delay_hit);
              show_spot(x+xdir,y+ydir);
              }
            if (change_summon_combat == 'Y') i = party_blow(&player.monster[1][0-monster]);
            else {
              i = u_random(player.party[0].attack);
              if (i < 1) i = 1;
              }
            player.party[0].hp -= i;
            if (player.party[0].hp <= 0) {
              player.map[sx][sy] = player.party[0].prev;
              show_spot(sx,sy);
              }
            }
          }
        /* ********************************** */
        /* ***** Else just move the mob ***** */
        /* ********************************** */
        else {
          if (valid(monster,x+xdir,y+ydir) == '*') { x+=xdir; y+=ydir; }
          player.map[ox][oy] = player.monster[1][0-monster].prev;
          player.monster[1][0-monster].prev = player.map[x][y];
          player.map[x][y] = monster;
          if (in_sight(ox,oy) == '*') show_spot(ox,oy);
          if (ox == player.x && oy == player.y) show_player();
          if (in_sight(x,y) == '*') show_spot(x,y);
          if (player.monster[1][0-monster].prev>=950 &&
              player.monster[1][0-monster].prev<970) {
            trigger_trap(x,y,player.monster[1][0-monster].prev);
            }
          }
        }
      }
  player.monster[1][0-monster].special |= MS_MOVED;
  }

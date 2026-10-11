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

void move_summoned() {
  Int32  i;
  char dir;
  Int32  x,y;
  Int32  xdir,ydir;
  Int32  dx,dy;
  NPC_REC* target;
  dx = 0;
  dy = 0;
  xdir = 0;
  ydir = 0;
  x = player.party[0].treasure & 0xff;
  y = (player.party[0].treasure >> 8) & 0xff;
  /* *************************************************** */
  /* ***** Find direction to target, if one exists ***** */
  /* *************************************************** */
  if (find_target(x,y,&dx,&dy)) {
    dir = find_path(x,y,dx,dy,'N');
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
  else {
    /* ******************************************** */
    /* ***** Move towards player if no target ***** */
    /* ******************************************** */
    dir = '5';
    xdir = 0;
    ydir = 0;
    dir = find_path(x,y,player.x,player.y,'N');

    /* Without the Summon Combat change the second try heads for (player.y, player.y) */
    if (dir == '5' && change_summon_combat == 'Y') dir = find_path(x,y,player.x,player.y,'Y');
    else if (dir == '5') dir = find_path(x,y,player.y,player.y,'Y');
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
  if (x+xdir == player.x && y+ydir == player.y) {
    xdir = 0;
    ydir = 0;
    }
  /* ********************************** */
  /* ***** Check if hit a monster ***** */
  /* ********************************** */
  if (player.map[x+xdir][y+ydir] < 0) {
    if ((player.monster[1][0-player.map[x+xdir][y+ydir]].special & MS_FRIENDLY) == 0) {
      target = &(player.monster[1][0-player.map[x+xdir][y+ydir]]);
      i = 50 + player.party[0].level * 2;
      if (target->sleep > 0) i += 50;
      if (target->fear > 0) i += 25;
      i -= target->level * 2;
      i -= target->defense * 2;
      if (i < 20) i = 20;
      if (i > 80) i = 80;
      if (u_random(100) < i) {
        if (in_sight(x+xdir,y+ydir) == '*') {
          show_sprite(x+xdir-player.map_x,y+ydir-player.map_y,100);
          delay(delay_hit);
          show_spot(x+xdir,y+ydir);
          }
        i = u_random(player.party[0].attack);
        if (i < 1) i = 1;
        target->hp -= i;
        dead_monster(target,'Y',x+xdir,y+ydir);
        }
      }
    }
  /* ******************************* */
  /* ***** Otherwise just move ***** */
  /* ******************************* */
  else if (passable(x+xdir,y+ydir,'N','N')) {
    player.map[x][y] = player.party[0].prev;
    show_spot(x,y);
    x += xdir;
    y += ydir;
    player.party[0].treasure = (y << 8) | x;
    player.party[0].prev = player.map[x][y];
    player.map[x][y] = 300;
    show_spot(x,y);
    }
  }


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

void make_map(char dir) {
  Int32  i,j,x,y;
  Int32  upx,upy;
  Int32  downx,downy;
  byte floor;
  Int32  treasurex,treasurey;
  Int32 see_stairs;
  Int32 see_traps;
  Int32 slot;
  OBJECT_REC* item;
  if (player.quests[player.dungeon_number] != 'Y') player.quests[player.dungeon_number] = 'E';
  see_stairs = (get_skill(SK_ORACLE) >= 10 && u_random(1000) < get_skill(SK_ORACLE)) ? -1 : 0;
  see_traps = (get_skill(SK_ORACLE) >= 10 && u_random(2000) < get_skill(SK_ORACLE)) ? -1 : 0;
  treasurex = -1;
  treasurey = -1;
  floor = 7;
  load_level();

//  for (x=0; x<101; x++)
//    for (y=0; y<101; y++) {
//      player.map[x][y] = current_dungeon.level[player.d_level-1].map[x][y];
//      player.known[x][y] = current_dungeon.level[player.d_level-1].known[x][y];
//      }
//  for (i=0; i<1000; i++)
//    player.map_objects[i] = current_dungeon.level[player.d_level-1].objects[i];
//  for (x=0; x<6; x++)
//    for (y=0; y<61; y++) {
//      player.monster[x][y] = current_dungeon.level[player.d_level-1].monsters[x][y];
//      }

  treasurex = current_dungeon.treasurex;
  treasurey = current_dungeon.treasurey;
  upx = current_dungeon.level[player.d_level-1].upx;
  upy = current_dungeon.level[player.d_level-1].upy;
  downx = current_dungeon.level[player.d_level-1].downx;
  downy = current_dungeon.level[player.d_level-1].downy;
  if (player.d_level != player.effect[PE_DEPTH]) {
    treasurex = -1;
    treasurey = -1;
    }
  /* ***************************************************** */
  /* ***** Place dungeon treasure if on bottom level ***** */
  /* ***************************************************** */
  if (player.gateway != '*') {
    if (links[player.dungeon_number].treasure < 1000)
      if (player.d_level == player.effect[PE_DEPTH] && player.quests[player.dungeon_number] != 'Y') {
        x = treasurex;
        y = treasurey;
        for (i=x-1; i<=x+1; i++)
          for (j=y-1; j<=y+1; j++)
            player.map[i][j] = 145;
        item = find_linked_item(links[player.dungeon_number].treasure);
        if (item != NULL) {
          player.map_objects[0] = *item;
          player.map[x][y] = 1000;
          }
        }
    if (links[player.dungeon_number].treasure > 1000 && player.d_level == player.effect[PE_DEPTH]) {
      x = treasurex;
      y = treasurey;
      for (i=x-1; i<=x+1; i++)
        for (j=y-1; j<=y+1; j++)
          player.map[i][j] = 145;
      player.map[x][y] = links[player.dungeon_number].treasure + 1000;
      }
    }

/* ******************************************* */
/* ***** Check for existance of an altar ***** */
/* ******************************************* */
  if (u_random(100) == 10) {
    x = u_random(80) + 10;
    y = u_random(80) + 10;
    while (player.map[x][y] != floor) {
      x = u_random(80) + 10;
      y = u_random(80) + 10;
      }
    player.map[x][y] = 901;
    }

  if (dir == 'U') {
    player.x = downx;
    player.y = downy;
    }
  if (dir == 'D') {
    player.x = upx;
    player.y = upy;
    }

/* ********************************************************** */
/* ***** Reveal stairs or traps if oracle skill allowed ***** */
/* ********************************************************** */
  if (see_stairs || see_traps) {
    for (y=0; y<100; y++)
      for (x=0; x<100; x++) {
        if (see_stairs && player.map[x][y] == S_DNSTAIR) player.known[x][y] = 1;
        if (see_stairs && player.map[x][y] == S_UPSTAIR) player.known[x][y] = 1;
        if (see_stairs && player.map[x][y] >= 800 && player.map[x][y] < 900)
          player.known[x][y] = 1;
        }
    }

/* ************************************************* */
/* ***** Place summoned creature if one exists ***** */
/* ************************************************* */
  if (player.party[0].hp > 0) {
    if (find_empty(player.x,player.y,10,dungeon_floor,&x,&y)) {
      player.party[0].prev = player.map[x][y];
      player.map[x][y] = 300;
      player.party[0].treasure = (y << 8) | x;
      }
    else {
      player.party[0].hp = 0;
      }
    }

  if (player.gateway != '*' && Rcs_Random(25000) < get_skill(SK_ORACLE)) {
    for (x=1; x<=100; x++)
      for (y=1; y<=100; y++) {
        player.known[x][y] = 1;
        }
    }
  if (player.gateway == '*') {
    if (player.d_level == 1) {
      strcpy(player.d_name,"The Gateway");
      for (x=3; x<=98; x++)
        for (y=3; y<=98; y++)
          if (player.map[x][y] == 21) player.map[x][y] = 39;
      }
    if (player.d_level == 2) {
      strcpy(player.d_name,"The Gateway");
      for (x=3; x<=98; x++)
        for (y=3; y<=98; y++)
          if (player.map[x][y] == 21) player.map[x][y] = 38;
      }
    if (player.d_level == 3) {
      strcpy(player.d_name,"The Gateway");
      for (x=3; x<=98; x++)
        for (y=3; y<=98; y++)
          if (player.map[x][y] == 21) player.map[x][y] = 41;
      }
    if (player.d_level == 4) {
      player.diff = 30;
      strcpy(player.d_name,"Realm of Eshter");
      copyMap(&player.map,18);
      player.x = 78;
      player.y = 89;
      stock_monsters(player.d_level,-1,-1,&(player.map),&(player.monster));
      player.map_x = player.x-8;
      player.map_y = player.y-8;
      if (player.map_x<1) player.map_x = 1;
      if (player.map_y<1) player.map_y = 1;
      if (player.map_x>84) player.map_x = 84;
      if (player.map_y>84) player.map_y = 84;
      }
    if (player.d_level == 5) {
      strcpy(player.d_name,"Castle Eshter");
      if (player.gateway != 'F') {
        slot = find_item_slot();
        if (slot < 0) return;
        item = find_linked_item(704);
        if (item != NULL) {
          x = u_random(80) + 10;
          y = u_random(80) + 10;
          while (player.map[x][y] != floor) {
            x = u_random(80) + 10;
            y = u_random(80) + 10;
            }
          player.map_objects[slot] = *item;
          player.map[x][y] = 1000+slot;
          }
        }
      }
    }
  }

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

NPC_REC* get_monster(Int32 x,Int32 y) {
  NPC_REC* ret;
  ret = NULL;
  if (x < 1 || y < 1 || x > 100 || y > 100) return ret;
  if (in_arena == 'Y') {
    if (player.map[x][y] > 400 && player.map[x][y] < 500)
      ret = &(player.monster[player.map[x][y]-400][a_mon]);
    } else {
    if (player.map[x][y] < 0)
      ret = &(player.monster[1][0-player.map[x][y]]);
    }
  return ret;
  }

/* The creature a spell's mon index names: in the arena's group, or on the level */
NPC_REC* monster_record(Int32 mon) {
  if (in_arena == 'Y') return &(player.monster[mon][a_mon]);
  return &(player.monster[1][mon]);
  }


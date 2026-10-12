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

/* The map cell holding a creature. Returns 0 when it is not on the map. */
static Int32 find_cell(NPC_REC* mon,Int32* x,Int32* y) {
  Int32 cx,cy;

  for (cx=1; cx<=100; cx++)
    for (cy=1; cy<=100; cy++)
      if (get_monster(cx,cy) == mon) {
        *x = cx;
        *y = cy;
        return -1;
        }

  return 0;
  }

/* A poisoned creature's tick, after its hp is taken. Without the Poison Kills change it dies on the player's cell. */
void poison_kill(NPC_REC* mon) {
  Int32 x,y;

  x = player.x;
  y = player.y;
  if (change_poison_kills == 'Y') find_cell(mon,&x,&y);

  dead_monster(mon,'Y',x,y);
  }

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

void teleport() {
  player.x = u_random(100) + 1;
  player.y = u_random(100) + 1;
  while (player.map[player.x][player.y] != currentFloor) {
    player.x = u_random(100) + 1;
    player.y = u_random(100) + 1;
    }
  center_view();
  last_target_x = 0;
  last_target_y = 0;
  }

/* The Teleport spell works on dungeon levels only. teleport() hunts for stone floor, which the upland, the village and
   the Realm of Eshter have little or none of, and the arena must not be left. */
static Int32 can_teleport() {
  if (in_upland == 'Y' || in_arena == 'Y' || player.in_city == 'Y') return 0;
  return !(player.gateway == '*' && player.d_level == 4);
  }

/* The Teleport spell, with the Teleport Spell change */
void teleport_spell() {
  if (!can_teleport()) {
    msg("Fizzle",9);
    return;
    }
  teleport();
  discover();
  show_screen();
  }


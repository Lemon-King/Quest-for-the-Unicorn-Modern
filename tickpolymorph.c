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

/* One turn of a polymorph: warns as it runs out, and takes back the monster's hit points when it ends */
void tick_polymorph(void) {
  char buffer[20];
  if (player.effect[PE_POLYTIME]>0) {
      player.effect[PE_POLYTIME]--;
      if (player.effect[PE_POLYTIME] == 10) msg("Poly ends 10 turns",15);
      if (player.effect[PE_POLYTIME] > 0 && player.effect[PE_POLYTIME] < 6) {
        sprintf(buffer,"Poly ends %d turns",player.effect[PE_POLYTIME]);
        msg(buffer,15);
        }
      if (player.effect[PE_POLYTIME] == 0) {
        msg("Polymorph ended",15);
        player.chp -= monsters[player.effect[PE_POLYMORPH]].hp;
        player.mhp -= monsters[player.effect[PE_POLYMORPH]].hp;
        if (player.chp < 0) player.chp = 0;
        show_side();
        }
    }
  }

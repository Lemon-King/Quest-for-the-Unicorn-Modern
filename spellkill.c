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

/* A creature killed by area49(), super(), drain() or r_missile(). Without the Spell Kills change it only adds its exp. */
void spell_kill(NPC_REC* target,Int32 x,Int32 y) {
  if (change_spell_kills != 'Y') {
    log_spell_slain(target);
    player.exp += target->exp;
    player.map[x][y] = target->prev;
    return;
    }

  if (in_arena == 'Y') {
    gain_exp(target,0);
    player.map[x][y] = target->prev;
    }
  else dead_monster(target,'Y',x,y);
  }

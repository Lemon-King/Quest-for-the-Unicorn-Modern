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

/* A creature killed by area49(), super(), drain() or r_missile(), at map cell x,y. Without the Spell Kills change it
   only adds its exp, with none of the loot, kill stats, win check or party share the other spells give through
   dead_monster() and gain_exp(). */
void spell_kill(NPC_REC* target,Int32 x,Int32 y) {
  char buffer[32];

  if (change_spell_kills != 'Y') {
    sprintf(buffer,"Exp=%.0f",target->exp);
    prompt(buffer);
    player.exp += target->exp;
    player.map[x][y] = target->prev;
    return;
    }

  if (in_arena == 'Y') {
    gain_exp(target->exp,0);
    player.map[x][y] = target->prev;
    }
  else dead_monster(target,'Y',x,y);
  }

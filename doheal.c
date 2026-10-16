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

/* Restores the player's hit points up to their maximum; returns the hit points gained */
Int32 heal_player(Int32 amount) {
  Int32 before;

  before = player.chp;
  player.chp += amount;
  if (player.chp > get_mhp()) player.chp = get_mhp();

  return player.chp - before;
  }

void do_heal(Int32 amount,Int32 who) {
  if (who == 0) {                                   /* player */
    log_heal(amount,heal_player(amount));
    }
  if (who > 0) {                                    /* party member */
    player.party[who].hp += amount;
    if (player.party[who].hp > player.party[who].level * 9) player.party[who].hp = player.party[who].level * 9;
    }
  }


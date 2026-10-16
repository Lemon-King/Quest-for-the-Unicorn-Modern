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

/* The experience for slaying a creature, shared with the living party; who is the party member who slew it, 0 the
   player */
void gain_exp(NPC_REC* slain,Int32 who) {
  Int32 i;
  Int32 sc;
  Int32  count;
  double amt;
  double player_amt;
  double portion;
  amt = slain->exp;
  if (has_intrinsic(A_DEXP)) amt *= 2;
  if (has_intrinsic(A_IEXP)) amt *= 1.15;
  amt *= get_exp_multiplier();
  count = 2;
  for (i=1; i<=5; i++)
    if (player.party[i].hp > 0) count++;
  portion  = amt / count;
  player_amt = portion;
  if (who == 0) player_amt += portion;
  player.exp += player_amt;
  log_slain(slain,player_amt);
  sc = (player.effect[PE_HEROIC] > 0) ? player.effect[PE_HEROIC] : 1;
  for (i=1; i<=5; i++)
    if (player.party[i].hp > 0) {
      player.party[i].exp += portion;
      if (i == who) player.party[i].exp += portion;
      if (player.party[i].exp >= needed(player.party[i].level +1)) {
        player.party[i].level++;
        player.party[i].attack = 2 * player.party[i].level * sc;
        player.party[i].defense = 2 * player.party[i].level * sc;
        if (player.party[i].mana > 0)
          player.party[i].mana = 25 * player.party[i].level;
        player.party[i].hp = player.party[i].level * 9 * sc;
        log_ally_level(&(player.party[i]));
        }
      }
  }


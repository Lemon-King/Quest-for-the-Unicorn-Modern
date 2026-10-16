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

void arena_party_heal(Int32 healer) {
  Int32 ch;
  Int32 diff;
  Int32 i;
  Int32 n;
  n = player.party[healer].mana;
  n /= 20;
  if (n < 2) n = 2;
  n = n / 2 + u_random(n/2);
  if (n < 1) n = 1;
  ch = 0;
  diff = get_mhp() - player.chp;
  for (i=1; i<6; i++) {
    if (player.party[i].hp > 0) {
      if ((player.party[i].level * 9 - player.party[i].hp) > diff) {
        ch = i;
        diff = player.party[i].level * 9 - player.party[i].hp;
        }
      }
    }
  if (ch == 0) {
    if (player.chp < get_mhp()) {
      heal_player(n);
      log_ally_heals_you(&(player.party[healer]),n);
      }
    }
  else {
      if (player.party[ch].hp < player.party[ch].level * 9) {
      do_heal(n,ch);
      log_ally_heals(&(player.party[healer]),&(player.party[ch]),n);
      }
    }
  }


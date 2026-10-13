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

/* The damage a monster's blow does to a party member: divided by NPC_DIV unless Full Party Combat is on */
Int32 party_blow(NPC_REC* attacker) {
  Int32 i;

  i = u_random(attacker->attack);
  if (party_combat_mode != 'F') i /= NPC_DIV;
  if (i < 1) i = 1;

  return i;
  }

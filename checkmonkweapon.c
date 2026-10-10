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

void check_monk_weapon() {
  if (player.invent[I_RHND].otype == O_NONE) player.weapon = player.level * 2 + 3;
  else if (player.invent[I_RHND].otype == O_DAGR)
    player.weapon = player.invent[I_RHND].damage/8;
  else if (player.invent[I_RHND].otype == O_EDGE)
    player.weapon = player.invent[I_RHND].damage/8;
  else if (player.invent[I_RHND].otype == O_POLE)
    player.weapon = player.invent[I_RHND].damage/4;
  else if (player.invent[I_RHND].otype == O_BLNT)
    player.weapon = player.invent[I_RHND].damage/4;
  else if (player.invent[I_RHND].otype == O_BOW)
    player.weapon = player.invent[I_RHND].damage/4;
  else player.weapon = 0;
  player.armour = player.level * 2 + 5;
  if (player.invent[I_SHLD].otype != O_NONE || 
      player.invent[I_ARMR].otype != O_NONE) player.armour = 0;
  }


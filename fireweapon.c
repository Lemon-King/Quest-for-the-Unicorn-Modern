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

void fire_weapon() {
  if (player.invent[I_RHND].otype == O_BOW) fire(get_weapon(),&(player.invent[I_RHND]));
  if (player.invent[I_RHND].otype == O_WAND) fire_wand(player.invent[I_RHND].damage);
//  if (player.invent[I_RHND].otype == O_PROJ) missile(player.invent[I_RHND].damage,0,0);
  if (player.invent[I_RHND].otype == O_PROJ) fire_wand(player.invent[I_RHND].damage);
  }


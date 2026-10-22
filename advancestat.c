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

void advance_stat() {
  Int32 i;
  i = ATTR_STRENGTH + u_random(ATTR_CHARISMA);
  if (u_random(100) < 40) {
    player.stat[i] ++;
    if (player.stat[i] > 99) player.stat[i] = 99;
    log_stat_gain(i);
    }
  }


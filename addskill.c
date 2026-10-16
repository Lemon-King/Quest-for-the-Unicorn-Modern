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

void add_skill(Int32 skill,Int32 num) {
  Int32 amount;
  Int32 start;
  double scale;
  double n;
  start = player.skill[skill] / 100000;
  amount = player.skill[skill] / 100000;
  scale = 5000;
  while (amount > 0) {
    scale *= 0.85;
    amount--;
    }
  n = num;
  n *= scale;
  if (n < 1) n = 1;
  player.skill[skill] += n;
  if (player.skill[skill] > 9900000) player.skill[skill] = 9900000;
  if ((player.skill[skill] / 100000) != start) log_skill_gain(skill);
  }



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

void adjust_abilities() {
  if (get_str() == 3) player.ability[1] = -3;
  if (get_str() >= 4 && get_str() <= 5) player.ability[1] = -2;
  if (get_str() >= 6 && get_str() <= 10) player.ability[1] = -1;
  if (get_str() >= 11 && get_str() <= 15) player.ability[1] = 0;
  if (get_str() >= 16 && get_str() <= 17) player.ability[1] = 1;
  if (get_str() >= 18 && get_str() <= 21) player.ability[1] = 2;
  if (get_str() >= 22 && get_str() <= 99) player.ability[1] = 3;

  if (get_dex() >= 3 && get_dex() <= 4) player.ability[1] -= 2;
  if (get_dex() >= 5 && get_dex() <= 8) player.ability[1] -= 1;
  if (get_dex() >= 16 && get_dex() <= 20) player.ability[1] += 1;
  if (get_dex() >= 21 && get_dex() <= 22) player.ability[1] += 2;
  if (get_dex() >= 23 && get_dex() <= 24) player.ability[1] += 3;
  if (get_dex() >= 25 && get_dex() <= 99) player.ability[1] += 4;

  if (get_dex() >= 3 && get_dex() <= 4) player.ability[2] = -3;
  if (get_dex() >= 5 && get_dex() <= 6) player.ability[2] = -2;
  if (get_dex() >= 7 && get_dex() <= 7) player.ability[2] = -1;
  if (get_dex() >= 8 && get_dex() <= 14) player.ability[2] = 0;
  if (get_dex() >= 15 && get_dex() <= 16) player.ability[2] = 1;
  if (get_dex() >= 17 && get_dex() <= 20) player.ability[2] = 2;
  if (get_dex() >= 21 && get_dex() <= 24) player.ability[2] = 3;
  if (get_dex() >= 25 && get_dex() <= 99) player.ability[2] = 4;

  if (get_str() >= 3 && get_str() <= 4) player.ability[3] = -2;
  if (get_str() >= 5 && get_str() <= 7) player.ability[3] = -1;
  if (get_str() >= 8 && get_str() <= 14) player.ability[3] = 0;
  if (get_str() >= 15 && get_str() <= 17) player.ability[3] = 1;
  if (get_str() >= 18 && get_str() <= 21) player.ability[3] = 2;
  if (get_str() >= 22 && get_str() <= 23) player.ability[3] = 3;
  if (get_str() >= 24 && get_str() <= 99) player.ability[3] = 4;

  if (get_con() >= 3 && get_con() <= 4) player.ability[4] = -2;
  if (get_con() >= 5 && get_con() <= 7) player.ability[4] = -1;
  if (get_con() >= 8 && get_con() <= 14) player.ability[4] = 0;
  if (get_con() >= 15 && get_con() <= 17) player.ability[4] = 1;
  if (get_con() >= 18 && get_con() <= 21) player.ability[4] = 2;
  if (get_con() >= 22 && get_con() <= 23) player.ability[4] = 3;
  if (get_con() >= 24 && get_con() <= 99) player.ability[4] = 4;

  if (get_wis() >= 3 && get_wis() <= 4) player.ability[5] = -2;
  if (get_wis() >= 5 && get_wis() <= 7) player.ability[5] = -1;
  if (get_wis() >= 8 && get_wis() <= 14) player.ability[5] = 0;
  if (get_wis() >= 15 && get_wis() <= 17) player.ability[5] = 1;
  if (get_wis() >= 18 && get_wis() <= 21) player.ability[5] = 2;
  if (get_wis() >= 22 && get_wis() <= 23) player.ability[5] = 3;
  if (get_wis() >= 24 && get_wis() <= 99) player.ability[5] = 4;

  if (get_dex() >= 3 && get_dex() <= 3) player.ability[6] = -4;
  if (get_dex() >= 4 && get_dex() <= 5) player.ability[6] = -3;
  if (get_dex() >= 6 && get_dex() <= 7) player.ability[6] = -2;
  if (get_dex() >= 8 && get_dex() <= 10) player.ability[6] = -1;
  if (get_dex() >= 11 && get_dex() <= 16) player.ability[6] = 0;
  if (get_dex() >= 17 && get_dex() <= 22) player.ability[6] = 1;
  if (get_dex() >= 23 && get_dex() <= 25) player.ability[6] = 2;
  if (get_dex() >= 26 && get_dex() <= 99) player.ability[6] = 3;

  if (get_dex() >= 3 && get_dex() <= 3) player.ability[7] = -5;
  if (get_dex() >= 4 && get_dex() <= 5) player.ability[7] = -4;
  if (get_dex() >= 6 && get_dex() <= 7) player.ability[7] = -3;
  if (get_dex() >= 8 && get_dex() <= 10) player.ability[7] = -2;
  if (get_dex() >= 11 && get_dex() <= 12) player.ability[7] = -1;
  if (get_dex() >= 13 && get_dex() <= 17) player.ability[7] = 0;
  if (get_dex() >= 18 && get_dex() <= 21) player.ability[7] = 1;
  if (get_dex() >= 22 && get_dex() <= 23) player.ability[7] = 2;
  if (get_dex() >= 24 && get_dex() <= 25) player.ability[7] = 3;
  if (get_dex() >= 26 && get_dex() <= 99) player.ability[7] = 4;

  if (get_dex() >= 3 && get_dex() <= 3) player.ability[8] = -3;
  if (get_dex() >= 4 && get_dex() <= 5) player.ability[8] = -2;
  if (get_dex() >= 6 && get_dex() <= 9) player.ability[8] = -1;
  if (get_dex() >= 10 && get_dex() <= 17) player.ability[8] = 0;
  if (get_dex() >= 18 && get_dex() <= 21) player.ability[8] = 1;
  if (get_dex() >= 22 && get_dex() <= 24) player.ability[8] = 2;
  if (get_dex() >= 25 && get_dex() <= 99) player.ability[8] = 3;

  if (get_int() >= 3 && get_int() <= 4) player.ability[9] = -2;
  if (get_int() >= 5 && get_int() <= 8) player.ability[9] = -1;
  if (get_int() >= 9 && get_int() <= 15) player.ability[9] = 0;
  if (get_int() >= 16 && get_int() <= 19) player.ability[9] = 1;
  if (get_int() >= 20 && get_int() <= 22) player.ability[9] = 2;
  if (get_int() >= 23 && get_int() <= 24) player.ability[9] = 3;
  if (get_int() >= 25 && get_int() <= 99) player.ability[9] = 4;

  if (get_con() >= 3 && get_con() <= 3) player.ability[9] -= 5;
  if (get_con() >= 4 && get_con() <= 4) player.ability[9] -= 4;
  if (get_con() >= 5 && get_con() <= 5) player.ability[9] -= 3;
  if (get_con() >= 6 && get_con() <= 6) player.ability[9] -= 2;
  if (get_con() >= 7 && get_con() <= 7) player.ability[9] -= 1;
  if (get_con() >= 19 && get_con() <= 20) player.ability[9] += 1;
  if (get_con() >= 21 && get_con() <= 23) player.ability[9] += 2;
  if (get_con() >= 24 && get_con() <= 99) player.ability[9] += 3;

  if (get_int() >= 3 && get_int() <= 4) player.ability[10] = -2;
  if (get_int() >= 5 && get_int() <= 7) player.ability[10] = -1;
  if (get_int() >= 8 && get_int() <= 14) player.ability[10] = 0;
  if (get_int() >= 15 && get_int() <= 17) player.ability[10] = 1;
  if (get_int() >= 18 && get_int() <= 21) player.ability[10] = 2;
  if (get_int() >= 22 && get_int() <= 23) player.ability[10] = 3;
  if (get_int() >= 24 && get_int() <= 99) player.ability[10] = 4;
  }

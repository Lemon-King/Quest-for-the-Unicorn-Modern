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

void add_gold(Int32 g) {
  Int32 m;
  Int32 a;
  a = (g / 10000);
  g -= (a * 10000);
  m = (g / 100);
  g -= (m * 100);
  player.gold_coins += g;
  player.mithril_coins += m;
  player.adamantite_coins += a;
  }

void sub_gold(Int32 g) {
  player.gold_coins -= g;
  while (player.gold_coins < 0) {
    player.gold_coins += 100;
    player.mithril_coins--;
    }
  while (player.mithril_coins < 0) {
    player.mithril_coins += 100;
    player.adamantite_coins--;
    }
  if (player.gold_coins < 0 || player.mithril_coins < 0 || player.adamantite_coins < 0) {
    player.gold_coins = 0;
    player.mithril_coins = 0;
    player.adamantite_coins = 0;
    }
  }

/* With the No Negative Amounts change, a typed amount that is not a number of 0 or more counts as 0. */
double typed_gold(double amount) {
  if (change_negative_amounts == 'Y' && !(amount >= 0)) return 0;
  return amount;
  }

/* "Gold = " and the gold you carry, padded so a smaller amount covers a larger one */
void show_gold(Int32 column,Int32 line) {
  char buffer[32];

  sprintf(buffer,"Gold = %-11d",get_gold());
  textcolor(14);
  gotoxy(column,line);
  txt_write(buffer);
  }

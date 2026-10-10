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

double buy_price(double price) {
  double discount;
  discount = 30.0 * (get_skill(SK_CHARM)/100.0);
  discount = 1.0 - (discount / 100.0);
  if (player.sex == 'F') discount -= 0.02;
  if (player.stat[6] >= 0 && player.stat[6] <= 7) discount += .16;
  if (player.stat[6] >= 8 && player.stat[6] <= 11) discount += .10;
  if (player.stat[6] >= 12 && player.stat[6] <= 14) discount += .06;
  if (player.stat[6] >= 17 && player.stat[6] <= 18) discount -= .02;
  if (player.stat[6] >= 19 && player.stat[6] <= 20) discount -= .04;
  if (player.stat[6] >= 21 && player.stat[6] <= 23) discount -= .06;
  if (player.stat[6] >= 24 && player.stat[6] <= 99) discount -= .10;
  if (classes[player.class-1].flags & CF_NOBLE) discount -= 0.15;
  price = price * discount;
  return price;
  }


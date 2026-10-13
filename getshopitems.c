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

void get_shop_items(Int32 otype1,Int32 otype2,Int32 otype3,Int32 otype4,Int32 otype5,Int32 otype6,Int32 rarity) {
  Int32 i;
  n_shop_items = 0;
  i = 0;
  while (i >= 0) {
    if (objects[i].control2 >= 0 &&
        (objects[i].flags & OF_FOR_SALE) != 0 &&
        ( objects[i].otype == otype1 ||
          objects[i].otype == otype2 ||
          objects[i].otype == otype3 ||
          objects[i].otype == otype4 ||
          objects[i].otype == otype5 ||
          objects[i].otype == otype6)) {
      shop_items[n_shop_items++] = objects[i];
      }
    if (objects[i].otype >= 99) i=-1;
      else i++;
    }
  sort_shop_items();
  }

/* Keeps the shop's items of max_level or lower, in their order */
void limit_shop_items(Int32 max_level) {
  Int32 i;
  Int32 kept;

  kept = 0;

  for (i=0; i<n_shop_items; i++)
    if (abs(shop_items[i].level) <= max_level) shop_items[kept++] = shop_items[i];

  n_shop_items = kept;
  }


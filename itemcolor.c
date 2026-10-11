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

Int32 item_color(OBJECT_REC* item) {
  if ((item->flags & OF_IDENT) == 0) return 3;
  if (abs(item->rarity) > 2500) return 7;       /* Common */
  if (abs(item->rarity) > 500) return 15;       /* Uncommon */
  if (abs(item->rarity) > 100) return 10;       /* Rare */
  if (abs(item->rarity) > 20) return 9;         /* Very Rare */
  if (abs(item->rarity) > 1) return 13;         /* Ultra Rare */

  return 14;
  }

void itemColor(OBJECT_REC* item) {
  if (item == NULL) return;

  textcolor(item_color(item));
  }


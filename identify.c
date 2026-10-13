/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Identifying an item without a scroll or a shop: the roll its first pickup makes, by its rarity and your Lore, and with
   the Improved Item Identification preference a common item identified as it is made, since that roll never fails. */

#include "header.h"

#define COMMON_RARITY   2500    /* rarer than this, the pickup roll can fail */

/* Never a cursed or a mutable item, which only a scroll, a shop or the identify intrinsic can reveal */
static Int32 may_identify(OBJECT_REC* item) {
  return item->effect != E_MUTA && item->level >= 0 && item->plus >= 0;
  }

/* The first pickup's roll, with g or G. An item that fails one band's roll tries the rarer bands' rolls too. */
void identify_on_pickup(OBJECT_REC* item) {
  if (!may_identify(item)) return;

  if (item->rarity > COMMON_RARITY) item->flags |= OF_IDENT;
  else if (item->rarity > 500 && (u_random(100)-get_skill(SK_LORE)*5) < 25) item->flags |= OF_IDENT;
  else if (item->rarity > 100 && (u_random(1000)-get_skill(SK_LORE)*10) < 25) item->flags |= OF_IDENT;
  else if (item->rarity > 5 && (u_random(10000)-get_skill(SK_LORE)*15) < 25) item->flags |= OF_IDENT;
  else if (item->rarity > 0 && (u_random(100000)-get_skill(SK_LORE)*20) < 25) item->flags |= OF_IDENT;
  }

/* With the Improved Item Identification preference, a new item on the floor that its first pickup is sure to identify */
void identify_common_item(OBJECT_REC* item) {
  if (improved_item_id != 'Y') return;

  if (may_identify(item) && item->rarity > COMMON_RARITY) item->flags |= OF_IDENT;
  }

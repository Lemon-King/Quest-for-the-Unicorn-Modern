/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Picking an item up, with g (inventory()) or G (pack_it()) */

#include "header.h"

/* The pickup trophies the item counts towards: its type, its own link, and a run of links */
static void count_pickup_trophies(OBJECT_REC* it) {
  Int32 i;

  i = 0;
  while (trophies[i].criteria_type < 999) {
    if (trophies[i].criteria_type == TC_PICKT) {
      if (it->otype == trophies[i].control[0]) stats.counts[trophies[i].control[2]]++;
      if (trophies[i].control[3] > 0)
        if (it->otype == trophies[i].control[3]) stats.counts[trophies[i].control[5]]++;
      }
    if (trophies[i].criteria_type == TC_PICKI) {
      if (it->link == trophies[i].control[0]) stats.counts[trophies[i].control[2]]++;
      if (trophies[i].control[3] > 0)
        if (it->link == trophies[i].control[3]) stats.counts[trophies[i].control[5]]++;
      }
    if (trophies[i].criteria_type == TC_PICKR) {
      if (it->link >= trophies[i].control[0] && it->link <= trophies[i].control[1]) {
        stats.counts[trophies[i].control[2]] |= (1 << (it->link - trophies[i].control[0]));
        }
      }
    i++;
    }
  }

/* key is g or G. An item's first pickup counts it in the rarity tallies and pickup trophies and rolls to identify it;
   with g it also adds a trading card to the collection, and every pickup with g sets a mutable item's level and makes
   a random recipe a recipe. The Unified Pickup change does those for G too; the quest treasure, which g also handles,
   pack_it() does itself. */
void pick_up_item(OBJECT_REC* it,char key) {
  Int32 full;

  full = (key == 'g' || change_unified_pickup == 'Y');

  if ((it->flags & OF_COUNTED) == 0) {
    if (it->otype == O_CARD && full) stats.counts[it->control] |= (1 << it->control2);
    count_rarity(it,key);
    identify_on_pickup(it);
    count_pickup_trophies(it);
    }
  if (full) {
    if (it->effect == E_MUTA) it->level = player.diff + player.d_level / 3;
    if (it->effect == E_SPCL && it->control == 79) convert_recipe(it);
    }
  it->flags |= OF_COUNTED;
  }

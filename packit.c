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

void pack_it(Int32 floor) {
  Int32  i;
  Int32  ct;
  char buffer[40];
  char packed[160];
  char flag;
  Int32  slot;
  Int32  pitem;
  OBJECT_REC* it;
  slot = 1;
  pitem = player.map[player.x][player.y] - 1000;

  /* A lore square (2000-2999) is no floor item; without the Lore Books change it reads past map_objects */
  if (change_lore_books == 'Y' && pitem >= 1000) return;
  it = &(player.map_objects[pitem]);
  if (it->otype == O_NONE) return;
  flag = ' ';
  while (flag == ' ') {
    if (player.pack[slot].otype == O_NONE) flag = '*';
    if (flag != '*') slot++;
    if (slot == 33) flag = '#';
    }
  if (flag != '#') {
    pick_up_item(it,'G');

    /* With the Unified Pickup change the quest treasure completes the quest when packed too, as g does it; the
       Gateway has no row in links[] for is_quest_treasure() to read */
    if (change_unified_pickup == 'Y' && player.dungeon_number != GATEWAY_DUNGEON && is_quest_treasure(it)) {
      player.quests[player.dungeon_number] = 'Y';
      it->flags |= OF_IDENT;
      }
    player.pack[slot] = player.map_objects[pitem];

    /* Without the Floor Items change the floor slot is never freed for reuse */
    if (change_floor_items == 'Y') it->otype = O_NONE;
    clear_prompt();

    /* With the Improved Interface only the item's name is in its rarity colour */
    if (improved_interface == 'Y') msg_named("Packed",get_name(&(player.pack[slot])),15,item_color(&(player.pack[slot])));
    else {
      sprintf(packed,"Packed %s",get_name(&(player.pack[slot])));
      msg(packed,item_color(&(player.pack[slot])));
      }
    player.map[player.x][player.y] = floor;
    show_spot(player.x,player.y);
    ct = 0;
    for (i=1; i<=32; i++)
      if (player.pack[i].otype != O_NONE) ct++;
    sprintf(buffer,"Pack contains %d items",ct);
    msg(buffer,15);
    show_player();
    }
  else {
    clear_prompt();
    msg("Pack full !!!",14);
    }
  }


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

/* The name of the item the player stands on, in the colour view mode shows it in, where view mode shows names */
void show_floor_item(void) {
  Int32       value;
  OBJECT_REC* item;
  char        name[160];

  hide_bottom_name();
  if (floor_item_names != 'Y') return;

  value = player.map[player.x][player.y];
  if (value < 1000 || value >= 2000) return;

  item = &(player.map_objects[value-1000]);
  if (item->otype == O_NONE) return;

  marked_item_name(item,name);
  show_bottom_name(name, floor_item_color(item));
  }

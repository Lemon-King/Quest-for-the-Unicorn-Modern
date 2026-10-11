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

#define NAME_WIDTH  38                /* view mode's longest name; 20 fit where it starts */

static char shown = 'N';

/* Clears the floor item's name if one is showing */
void hide_floor_item(void) {
  if (shown != 'Y') return;
  outtextxyclr(screen_width - 8*NAME_WIDTH, screen_height-15, "                                      ");
  shown = 'N';
  }

/* With the Floor Item Names preference, the name of the item the player stands on, in its inventory colour, where view
   mode shows names; a name too long for that spot moves left to end at the screen's edge */
void show_floor_item(void) {
  Int32 value;
  Int32 x;
  OBJECT_REC* item;
  char buffer[NAME_WIDTH+1];
  hide_floor_item();
  if (floor_item_names != 'Y') return;
  value = player.map[player.x][player.y];
  if (value < 1000 || value >= 2000) return;
  item = &(player.map_objects[value-1000]);
  if (item->otype == O_NONE) return;
  strncpy(buffer, get_name(item), NAME_WIDTH);
  buffer[NAME_WIDTH] = 0;
  x = screen_width - 160;
  if (x + 8*(Int32)strlen(buffer) > screen_width) x = screen_width - 8*(Int32)strlen(buffer);
  setcolor(item_color(item));
  outtextxyclr(x, screen_height-15, buffer);
  shown = 'Y';
  }

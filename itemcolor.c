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

/* The colour of the item's rarity, identified or not */
static Int32 rarity_color(OBJECT_REC* item) {
  if (abs(item->rarity) > 2500) return 7;       /* Common */
  if (abs(item->rarity) > 500) return 15;       /* Uncommon */
  if (abs(item->rarity) > 100) return 10;       /* Rare */
  if (abs(item->rarity) > 20) return 9;         /* Very Rare */
  if (abs(item->rarity) > 1) return 13;         /* Ultra Rare */

  return 14;
  }

/* Whether the item shows as unidentified by its colour, as it does without the Improved Item Identification
   preference, whose * marks it instead (marked_item_name()) */
static Int32 colored_unknown(OBJECT_REC* item) {
  return (item->flags & OF_IDENT) == 0 && improved_item_id != 'Y';
  }

/* An item's colour where you carry it: its rarity colour, or cyan while it is unidentified */
Int32 item_color(OBJECT_REC* item) {
  if (colored_unknown(item)) return 3;

  return rarity_color(item);
  }

void itemColor(OBJECT_REC* item) {
  if (item == NULL) return;

  textcolor(item_color(item));
  }

/* An item's colour on the floor: its rarity colour, or yellow while it is unidentified, the colour 7.0's look command
   gave every name */
Int32 floor_item_color(OBJECT_REC* item) {
  if (colored_unknown(item)) return CLR_YELLOW;

  return rarity_color(item);
  }

/* An item's name on the floor or where you keep it, after a * while it is unidentified with the Improved Item
   Identification preference */
void marked_item_name(OBJECT_REC* item,char* name) {
  Int32 marked;

  marked = (improved_item_id == 'Y' && (item->flags & OF_IDENT) == 0);
  sprintf(name,"%s%s",marked ? "*" : "",get_name(item));
  }

/* At the cursor: before and after in color, and between them the item's name, in its rarity colour with the Improved
   Interface preference, or else in color too, as the game first wrote it */
void write_named(char* before,OBJECT_REC* item,char* name,char* after,Int32 color) {
  textcolor(color);
  txt_write(before);
  if (improved_interface == 'Y') itemColor(item);
  txt_write(name);
  textcolor(color);
  txt_write(after);
  }


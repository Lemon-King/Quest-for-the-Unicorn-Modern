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

/* Each band's name, as the item card writes it, and its colour, from Special up */
static char* band_names[NUM_RARITY_BANDS]  = { "Special", "Ultra Rare", "Very Rare", "Rare", "Uncommon", "Common" };
static Int32 band_colors[NUM_RARITY_BANDS] = { 14, 13, 9, 10, 15, 7 };

/* The item's rarity band, by the bands its card names */
Int32 rarity_band(OBJECT_REC* item) {
  Int32 rarity;

  rarity = abs(item->rarity);
  if (rarity > 2500) return RB_COMMON;
  if (rarity > 500) return RB_UNCOMMON;
  if (rarity > 100) return RB_RARE;
  if (rarity > 5) return RB_VERY_RARE;
  if (rarity > 0) return RB_ULTRA_RARE;

  return RB_SPECIAL;
  }

char* band_name(Int32 band) {
  return band_names[band];
  }

Int32 band_color(Int32 band) {
  return band_colors[band];
  }

/* Gear known for what it is more than for its rarity, once identified: a unique or legendary item, or legacy gear */
static Int32 is_named_gear(OBJECT_REC* item) {
  if ((item->flags & OF_IDENT) == 0) return 0;

  return (item->flags & OF_UNIQUE) != 0 || item->effect == E_LGND || item->effect == E_LGCY;
  }

/* The colour of the item's rarity, identified or not: with the Improved Items preference its band's, or yellow for
   named gear; without it, 7.0's bands, which call 6 to 20 ultra rare and colour 1 and below yellow */
static Int32 rarity_color(OBJECT_REC* item) {
  if (improved_items == 'Y') return is_named_gear(item) ? CLR_YELLOW : band_color(rarity_band(item));

  if (abs(item->rarity) > 2500) return 7;       /* Common */
  if (abs(item->rarity) > 500) return 15;       /* Uncommon */
  if (abs(item->rarity) > 100) return 10;       /* Rare */
  if (abs(item->rarity) > 20) return 9;         /* Very Rare */
  if (abs(item->rarity) > 1) return 13;         /* Ultra Rare */

  return 14;
  }

/* Whether the item shows as unidentified by its colour, as it does without the Improved Items preference, whose *
   marks it instead (marked_item_name()) */
static Int32 colored_unknown(OBJECT_REC* item) {
  return (item->flags & OF_IDENT) == 0 && improved_items != 'Y';
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

/* An item's name on the floor or where you keep it, after a * while it is unidentified with the Improved Items
   preference */
void marked_item_name(OBJECT_REC* item,char* name) {
  Int32 marked;

  marked = (improved_items == 'Y' && (item->flags & OF_IDENT) == 0);
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


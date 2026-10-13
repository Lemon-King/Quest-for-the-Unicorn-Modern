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

void show_lore(Int32 item) {
  Int32 i;
  MESSAGE general;
  i = item;
  general = lore[i];
  player.lore[i] = -1;
  scroll_text(general.message);
  }


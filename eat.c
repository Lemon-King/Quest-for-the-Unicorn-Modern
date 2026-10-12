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

/* The e key in dungeons and on the overworld: eats the first food carried, then the first in the pack */
void eat(void) {
  OBJECT_REC* item;
  Int32 i;

  if (player.health > 100) {
    clear_prompt();
    prompt("Too full to eat any more");
    return;
    }

  item = NULL;
  for (i=1; i<11; i++)
    if (player.invent[i].otype == O_FOOD) {
      item = &(player.invent[i]);
      i = 99;
      }
  if (item == NULL) {
    for (i=1; i<=32; i++)
      if (player.pack[i].otype == O_FOOD) {
        item = &(player.pack[i]);
        i = 99;
        }
    }

  if (item == NULL) {
    clear_prompt();
    prompt("Nothing to Eat!!!");
    return;
    }

  if (item->charges != -1) item->charges--;

  /* Without the Last Bite change the last bite is cleared before perform_effect() counts it as a meal */
  if (item->charges == 0 && change_last_bite != 'Y') item->otype = O_NONE;
  perform_effect(item);
  show_side();
  }

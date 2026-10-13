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

/* An emptied slot keeps its item's link. Without the Possessions change such a slot still counts as holding it. */
static Int32 holds(OBJECT_REC* slot,Int32 link) {
  if (slot->link != link) return 0;
  return (change_possessions != 'Y' || slot->otype != O_NONE);
  }

Int32 possesses(Int32 link) {
  Int32 i;
  Int32 j;
  if (link == 0) return 0;
  for (i=0; i<20; i++) if (holds(&player.invent[i],link)) return -1;
  for (i=0; i<33; i++) if (holds(&player.pack[i],link)) return -1;
  for (i=0; i<1000; i++) if (holds(&player.hole[i],link)) return -1;
  for (i=0; i<5; i++)
    for (j=0; j<500; j++)
      if (holds(&player.houses[i].inventory[j],link)) return -1;
  return 0;
  }


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

Int32 possesses(Int32 link) {
  Int32 i;
  Int32 j;
  if (link == 0) return 0;
  for (i=0; i<20; i++) if (player.invent[i].link == link) return -1;
  for (i=0; i<33; i++) if (player.pack[i].link == link) return -1;
  for (i=0; i<1000; i++) if (player.hole[i].link == link) return -1;
  for (i=0; i<5; i++)
    for (j=0; j<500; j++)
      if (player.houses[i].inventory[j].link == link) return -1;
  return 0;
  }


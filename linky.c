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

char is_valley_of_fear(Int32 i) {
  return (links[i].region == 7 && links[i].x == 80 && links[i].y == 90 && links[i].typ == 11) ? 'Y' : 'N';
  }

/* The row of links[] y where a town or dungeon is on its region's map. The Valley of Fear's row says (80,90), a
   mountain on region 7's map; its entrance tile is at (80,97), so without the Valley of Fear change it cannot be entered
   and the compass points at the mountain. Its x is right. */
Int32 link_y(Int32 i) {
  if (change_valley_of_fear == 'Y' && is_valley_of_fear(i) == 'Y') return 97;

  return links[i].y;
  }

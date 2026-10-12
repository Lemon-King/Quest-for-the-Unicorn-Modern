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
  return (links[i].region == 7 &&
          links[i].x == 80 &&
          links[i].y == 90 &&
          links[i].typ == 11) ? 'Y' : 'N';
  }

/* links[i]'s y. The Valley of Fear's row gives a mountain at y 90; its entrance is at y 97. */
Int32 link_y(Int32 i) {
  if (change_valley_of_fear == 'Y' && is_valley_of_fear(i) == 'Y') return 97;

  return links[i].y;
  }

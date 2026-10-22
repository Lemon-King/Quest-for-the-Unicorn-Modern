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

/* The name of the town or dungeon at x,y on region's overworld map, or NULL */
char* link_name(Int32 region,Int32 x,Int32 y) {
  Int32 i;

  for (i=0; links[i].region < 90; i++)
    if (links[i].region == region && links[i].x == x && link_y(i) == y) return links[i].name;

  return NULL;
  }

/* The name of the town or dungeon whose entrance is the square x,y of the overworld you are on, past its region's
   edges on the joined overworld, or NULL */
char* entrance_name(Int32 x,Int32 y) {
  Int32 value;

  value = square_value(x,y);
  if (value != 5 && value != 11) return NULL;

  return link_name(land_region(&x,&y),x,y);
  }

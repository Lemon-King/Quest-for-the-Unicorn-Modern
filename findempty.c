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

Int32 find_empty(Int32 cx,Int32 cy,Int32 range,Int32 floor,Int32* dx,Int32 * dy) {
  Int32 i;
  Int32 x,y;
  for (i=1; i<range; i++)
    for (y=cy-i; y<=cy+i; y++)
      for (x=cx-i; x<=cx+i; x++)
        if (x > 1 && x < 100 && y > 1 && y < 100)
          if ((x != player.x || y != player.y) && player.map[x][y] == floor) {
            *dx = x;
            *dy = y;
            return -1;
            }
  return 0;
  }


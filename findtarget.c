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

Int32 find_target(Int32 x,Int32 y,Int32* tx,Int32* ty) {
  Int32 ix,iy;
  double d2;
  double dist;
  dist = 99999;
  for (iy = y-20; iy <= y+20; iy++)
    for (ix = x-20; ix <= x+20; ix++)
      if (ix >1 && ix < 100 && iy > 1 && iy < 100)
        if (player.map[ix][iy] < 0)
          if (los(x,y,ix,iy,'Y')) {
            d2 = ((x-ix)*(x-ix)) + ((y-iy)*(y-iy));
            if (d2 < dist) {
              dist = d2;
              *tx = ix;
              *ty = iy;
              }
            }
  return (dist != 99999) ? -1 : 0;
  }


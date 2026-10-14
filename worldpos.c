/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* A position across the whole overworld, whose regions lie 4 across and 3 down: 1-4, 5-8, 9-12. */

#include "header.h"

Int32 getPX(Int32 r,Int32 x) {
  return (((r - 1) % WORLD_COLUMNS) * REGION_SQUARES) + x;
  }

Int32 getPY(Int32 r,Int32 y) {
  return (((r - 1) / WORLD_COLUMNS) * REGION_SQUARES) + y;
  }

/* The region holding the overworld position px,py, or 0 off the world */
Int32 world_region(Int32 px,Int32 py) {
  if (px < 1 || py < 1 || px > WORLD_WIDTH || py > WORLD_HEIGHT) return 0;

  return ((py - 1) / REGION_SQUARES) * WORLD_COLUMNS + (px - 1) / REGION_SQUARES + 1;
  }

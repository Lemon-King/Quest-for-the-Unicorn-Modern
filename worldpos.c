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
  return (((r - 1) % 4) * 100) + x;
  }

Int32 getPY(Int32 r,Int32 y) {
  return (((r - 1) / 4) * 100) + y;
  }

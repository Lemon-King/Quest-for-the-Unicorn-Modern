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

Int32 u_random(Int32 range) {
  float r;
  if (range < 0) range = 0;
  r=(float)rand();
  r/=RAND_MAX;
  r-=(Int32)r;
  r*=range;
  return (Int32)r;
  }


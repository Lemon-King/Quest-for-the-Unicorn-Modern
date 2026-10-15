/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The eight steps the direction keys take, clockwise from north, as d from 0 to 7 */

#include "header.h"

#define STEP_COUNT  8

static const Int32 STEP_X[STEP_COUNT]   = { 0, 1, 1, 1, 0, -1, -1, -1 };
static const Int32 STEP_Y[STEP_COUNT]   = { -1, -1, 0, 1, 1, 1, 0, -1 };
static const char  STEP_KEY[STEP_COUNT] = { '8', '9', '6', '3', '2', '1', '4', '7' };

Int32 step_x(Int32 d) {
  return STEP_X[d];
  }

Int32 step_y(Int32 d) {
  return STEP_Y[d];
  }

char step_key(Int32 d) {
  return STEP_KEY[d];
  }

/* The step key takes, or -1 for a key that is not a direction */
Int32 key_step(char key) {
  Int32 d;

  for (d=0; d<STEP_COUNT; d++)
    if (STEP_KEY[d] == key) return d;

  return -1;
  }

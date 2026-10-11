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

/* The lore or gen row a find_lore() or find_gen() result names. They return the matched row plus one, 0 for none;
   without the Lore Search change that value is used as the row, so the row after the match is shown and learned. */
Int32 found_row(Int32 found) {
  if (change_lore_search == 'Y') return found - 1;
  return found;
  }

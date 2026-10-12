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

/* find_lore() and find_gen() return the row plus one; without the Lore Search change that is used as the row */
Int32 found_row(Int32 found) {
  if (change_lore_search == 'Y') return found - 1;
  return found;
  }

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

/* The player.monster[1][] index of a creature's map cell, which holds it negated; without the Spell Targets change
   the cell is used as it is */
Int32 spell_target(Int32 cell) {
  if (change_spell_targets == 'Y') return -cell;
  return cell;
  }

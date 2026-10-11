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

/* The player.monster[1][] index of the creature in a map cell outside the arena. The map holds creatures as negative
   numbers; without the Spell Targets change the cell is used as it is, so the spell lands on a record before the
   creature's. */
Int32 spell_target(Int32 cell) {
  if (change_spell_targets == 'Y') return -cell;
  return cell;
  }

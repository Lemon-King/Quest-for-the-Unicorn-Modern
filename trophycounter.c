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

/* The stats counter in a trophy's control[slot]. Trophy 34 counts meat in Meals Eaten (20); with the Meat and Potatoes
   change it uses the unused counter 24. */
Int32 trophy_counter(Int32 trophy,Int32 slot) {
  if (change_meat_potatoes == 'Y' && trophy == 34 && slot == 2) return 24;
  return trophies[trophy].control[slot];
  }

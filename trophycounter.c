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

/* The stats counter a trophy keeps in control[slot], 2 or 5. Trophy 34, Meat and Potatoes, keeps its meat count in
   counter 20, Meals Eaten, so meat counts twice as a meal and potatoes alone earn it. With the Meat and Potatoes change
   it counts meat in counter 24, between eggs (23) and potatoes (25), which nothing else uses. */
Int32 trophy_counter(Int32 trophy,Int32 slot) {
  if (change_meat_potatoes == 'Y' && trophy == 34 && slot == 2) return 24;
  return trophies[trophy].control[slot];
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The rarity tallies an item's first pickup counts in, which the Eye for Treasure, Treasure Hunter and Lucky!
   trophies and the stats screen read. */

#include "header.h"

#define NUM_BANDS   5

/* A tally counts an item whose rarity is above above and at most most */
typedef struct {
  Int32 tally;
  Int32 above;
  Int32 most;
  } RARITY_BAND;

/* Picking up with g counts by the bands the item card names */
static const RARITY_BAND get_bands[NUM_BANDS] = {
  { STAT_COMMONS,     2500, INT32_MAX },
  { STAT_UNCOMMONS,    500,      2500 },
  { STAT_RARES,        100,       500 },
  { STAT_VERY_RARES,     5,       100 },
  { STAT_ULTRA_RARES,    0,         5 }
  };

/* Packing with G counts by its own bands, which leave a rarity of 5 uncounted */
static const RARITY_BAND pack_bands[NUM_BANDS] = {
  { STAT_COMMONS,     2500, INT32_MAX },
  { STAT_UNCOMMONS,    100,      2500 },
  { STAT_RARES,         50,       100 },
  { STAT_VERY_RARES,     5,        50 },
  { STAT_ULTRA_RARES,    0,         4 }
  };

/* Rarity Fix's Band C: packing's bands, with a rarity of 5 ultra rare */
static const RARITY_BAND band_c[NUM_BANDS] = {
  { STAT_COMMONS,     2500, INT32_MAX },
  { STAT_UNCOMMONS,    100,      2500 },
  { STAT_RARES,         50,       100 },
  { STAT_VERY_RARES,     5,        50 },
  { STAT_ULTRA_RARES,    0,         5 }
  };

/* key is the one that picked the item up, g or G. Without the Rarity Fix change each counts by its own bands; Band A
   counts both by g's, Band B both by G's, and Band C both by Band C's. */
void count_rarity(OBJECT_REC* item,char key) {
  const RARITY_BAND* bands;
  Int32              i;

  bands = (key == 'g') ? get_bands : pack_bands;
  if (change_rarity_fix == 'A') bands = get_bands;
  if (change_rarity_fix == 'B') bands = pack_bands;
  if (change_rarity_fix == 'C') bands = band_c;

  for (i=0; i<NUM_BANDS; i++)
    if (item->rarity > bands[i].above && item->rarity <= bands[i].most) stats.counts[bands[i].tally]++;
  }

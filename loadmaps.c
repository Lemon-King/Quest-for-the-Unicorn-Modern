/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 * The world's maps, read from the maps folder in the working directory. MAP_FILES is in the order the game indexes
 * maps[] by: index 0 repeats map11, regions 1-12 follow, then the towns (13-17) and Eshter's realm (18).
 */

#include "header.h"

static char* MAP_FILES[] = {
  "maps\\map11.map",
  "maps\\map11.map",
  "maps\\map12.map",
  "maps\\map13.map",
  "maps\\map14.map",
  "maps\\map21.map",
  "maps\\map22.map",
  "maps\\map23.map",
  "maps\\map24.map",
  "maps\\map31.map",
  "maps\\map32.map",
  "maps\\map33.map",
  "maps\\map34.map",
  "maps\\radon.map",
  "maps\\generic.map",
  "maps\\village.map",
  "maps\\marduk.map",
  "maps\\nichor.map",
  "maps\\eshter.map"
  };

_Static_assert(sizeof(MAP_FILES) / sizeof(MAP_FILES[0]) == MAP_COUNT, "one file per map");

/* Fills world[0] to world[MAP_COUNT-1] and returns 1, or returns 0 with error from the first map that failed */
Int32 load_maps(MAP_TYPE* world,char* error,Int32 error_size) {
  Int32 i;
  for (i=0; i<MAP_COUNT; i++)
    if (read_map(MAP_FILES[i],&world[i],error,error_size) == 0) return 0;
  return 1;
  }

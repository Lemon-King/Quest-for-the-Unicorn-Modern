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

static void show_tile(Int32 x,Int32 y,Int32 sprite) {
  show_sprite(x-player.map_x,y-player.map_y,sprite);
  }

/* The sprite of a thing lying on the floor, or 0 when the value is a tile of its own. */
static Int32 placed_sprite(Int32 value) {
  if (value >= 1000 && value < 2000) return player.map_objects[value-1000].sprite;
  if (value > 2000) return 88;                                   /* Lore book            */
  if (value >= 903 && value <= 910) return value - 586;           /* Runestones           */
  if (value >= 950 && value <= 970) return 330;                   /* Traps                */
  switch (value) {
    case 40:     return 40;                                       /* Trap                 */
    case 79:     return 79;                                       /* Chest                */
    case 84:     return 84;                                       /* Torch                */
    case 143:    return 342;                                      /* Summoned snake       */
    case 145:    return 359;                                      /* Treasure force field */
    case 536:    return 350;                                      /* Gacha machine        */
    case 901:    return 315;                                      /* Altar                */
    case 902:    return 316;                                      /* Used altar           */
    case S_SALE: return S_SALE;
    case S_SOLD: return S_SOLD;
    }
  return 0;
  }

/* Signs stand on grass. Anything else lies on stone, or with enhanced sprites on the level's floor. */
static Int32 placed_floor(Int32 value) {
  if (value == S_SALE || value == S_SOLD) return S_GRASS;
  return (enhanced_sprites == 'Y') ? currentFloor : 7;
  }

/* The tile under a creature, from the map value it covered when it stepped there. */
static Int32 ground_tile(Int32 under) {
  if (placed_sprite(under) != 0) return placed_floor(under);
  return under;
  }

/* Without enhanced sprites an awake creature stands on stone, unless it is an arena group,
   and a sleeping one has no backdrop. */
static void show_creature(Int32 x,Int32 y,NPC_REC* creature,char on_stone) {
  if (enhanced_sprites == 'Y')
    show_tile(x,y,ground_tile(creature->prev));
  else if (creature->sleep == 0 && on_stone == 'Y')
    show_tile(x,y,7);

  show_tile(x,y,(creature->sleep == 0) ? creature->sprite : 36);
  }

void show_spot(Int32 x,Int32 y) {
  Int32 value;

  /* The joined overworld draws squares past the region's edges too */
  if (seamless_world()) {
    show_land_square(x,y);
    return;
    }

  if (x < 1 || y < 1 || x > 100 || y > 100) return;
  if (in_sight(x,y) != '*') return;
  if (can_see() == 0 || player.known[x][y] == 0) {
    show_tile(x,y,10);
    return;
    }

  value = player.map[x][y];
  if (value < 0)
    show_creature(x,y,&player.monster[1][-value],'Y');
  else if (value >= 300 && value < 400)
    show_creature(x,y,&player.party[value-300],'Y');
  else if (value > 400 && value < 500)
    show_creature(x,y,&player.monster[value-400][a_mon],'N');
  else if (placed_sprite(value) != 0) {
    show_tile(x,y,placed_floor(value));
    show_tile(x,y,placed_sprite(value));
    }
  else
    show_tile(x,y,value);
  }

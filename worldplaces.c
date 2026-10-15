/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The places the world map marks: you, then each town, dungeon, village, the Gateway and your docked ship on a square
   you have found, down each column of the world in turn. A place on your own square names you instead of being
   marked twice. */

#include "header.h"

#define TOWN_TILE        5
#define DUNGEON_TILE    11
#define SHIP_TILE       12
#define VILLAGE_TILE    17
#define GATEWAY_OPEN    31
#define GATEWAY_SHUT    32

/* The name a tile gives the place at overworld position px,py, or NULL for a tile that is not a place */
static char* place_name(Int32 tile,Int32 px,Int32 py) {
  Int32 region;
  char* name;

  switch (tile) {
    case TOWN_TILE:
    case DUNGEON_TILE:
      region = world_region(px,py);
      name = link_name(region,px - getPX(region,0),py - getPY(region,0));
      if (name != NULL) return name;
      return (tile == TOWN_TILE) ? "Town" : "Dungeon";
    case VILLAGE_TILE: return "Village";
    case GATEWAY_OPEN:
    case GATEWAY_SHUT: return "Gateway";
    case SHIP_TILE:    return "Your ship";
    }

  return NULL;
  }

static void add_place(WORLD_PLACES* places,Int32 px,Int32 py,Int32 sprite,char* name) {
  WORLD_PLACE* place;

  if (places->count == MAX_WORLD_PLACES) return;

  place = &places->list[places->count++];
  place->px = px;
  place->py = py;
  place->sprite = sprite;
  sprintf(place->name,"%.32s",name);
  }

/* You are first, as your figure, or as your ship while you sail */
void find_world_places(WORLD_PLACES* places) {
  WORLD_PLACE* you;
  Int32        px,py;
  Int32        tile;
  char*        name;

  places->count = 0;
  add_place(places,getPX(player.region,player.x),getPY(player.region,player.y),
            (player.inship == ' ') ? rider_sprite() : SHIP_TILE,"You");
  you = &places->list[0];

  for (px=1; px<=WORLD_WIDTH; px++)
    for (py=1; py<=WORLD_HEIGHT; py++) {
      if (!world_known(px,py)) continue;

      tile = world_tile(px,py);
      name = place_name(tile,px,py);
      if (name == NULL) continue;

      if (px == you->px && py == you->py) sprintf(you->name,"%.32s",name);
        else add_place(places,px,py,tile,name);
      }
  }

/* The place nearest from in direction dx,dy: within 45 degrees of it, scored by how far ahead plus twice how far
   aside, the first in the list on a tie. Returns from when there is none. */
Int32 next_world_place(const WORLD_PLACES* places,Int32 from,Int32 dx,Int32 dy) {
  Int32 best,best_score;
  Int32 vx,vy;
  Int32 ahead,aside,score;
  Int32 i;

  best = from;
  best_score = 0;
  for (i=0; i<places->count; i++) {
    if (i == from) continue;

    vx = places->list[i].px - places->list[from].px;
    vy = places->list[i].py - places->list[from].py;
    ahead = vx * dx + vy * dy;
    aside = abs(vx * dy - vy * dx);
    if (ahead <= 0 || aside > ahead) continue;

    score = ahead + 2 * aside;
    if (best == from || score < best_score) {
      best = i;
      best_score = score;
      }
    }

  return best;
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The overworld as one map, for the Seamless World Map preference. Only the region you are in lives in player.map, so
   its squares keep their numbers and the squares past its edges are its neighbours': x 0 is the last column of the
   region to the west, x 101 the first of the region to the east. Without the preference every square is the region's
   own, as before. */

#include "header.h"

#define SHIP_TILE  12

/* The overworld you walk is drawn as one map; the arena has its own */
Int32 seamless_world(void) {
  return (seamless_world_map != 'N' && in_upland == 'Y' && in_arena != 'Y');
  }
/* Whether land square x,y lies past the region's edges, in a neighbour */
static Int32 beyond_region(Int32 x,Int32 y) {
  if (!seamless_world()) return 0;

  return (x < 1 || x > REGION_SQUARES || y < 1 || y > REGION_SQUARES);
  }

/* The region holding land square x,y, whose own square there it makes x,y; 0 off the world */
Int32 land_region(Int32* x,Int32* y) {
  Int32 px,py;
  Int32 region;

  if (!seamless_world()) return player.region;

  px = getPX(player.region,*x);
  py = getPY(player.region,*y);
  region = world_region(px,py);
  if (region == 0) return 0;

  *x = px - getPX(region,0);
  *y = py - getPY(region,0);

  return region;
  }

/* The Gateway's entrance: seen with A_GATE, and open with A_GACT as well; 0 while it does not show */
Int32 gateway_tile(void) {
  if (player.gateway != ' ' || !has_intrinsic(A_GATE)) return 0;

  return has_intrinsic(A_GACT) ? 31 : 32;
  }

/* Square x,y of a region other than yours, as up_land() would show it on entering: your docked ship and the Gateway
   stand on its map */
static Int32 region_tile(Int32 region,Int32 x,Int32 y) {
  if (player.inship == ' ' && player.ship_r == region && player.ship_x == x && player.ship_y == y) return SHIP_TILE;
  if (region == GATEWAY_REGION && x == GATEWAY_X && y == GATEWAY_Y && gateway_tile() != 0) return gateway_tile();

  return maps[region][y][x];
  }

/* The value of map square x,y; off the world, 0 */
Int32 square_value(Int32 x,Int32 y) {
  Int32 region;

  if (!beyond_region(x,y)) return player.map[x][y];

  region = land_region(&x,&y);
  if (region == 0) return 0;

  return region_tile(region,x,y);
  }

/* Where the map keeps whether square x,y is known, or NULL off the world */
static byte* known_square(Int32 x,Int32 y) {
  Int32 region;

  if (!beyond_region(x,y)) return &(player.known[x][y]);

  region = land_region(&x,&y);
  if (region == 0) return NULL;

  return &(known_maps[region][x][y]);
  }

Int32 square_known(Int32 x,Int32 y) {
  byte* known;

  known = known_square(x,y);

  return (known != NULL && *known != 0);
  }

/* Marks land square x,y known. Returns 1 when it was not known before. */
Int32 reveal_land(Int32 x,Int32 y) {
  byte* known;

  if (!seamless_world() && (x < 1 || x > REGION_SQUARES || y < 1 || y > REGION_SQUARES)) return 0;

  known = known_square(x,y);
  if (known == NULL || *known != 0) return 0;

  *known = 1;

  return 1;
  }

/* passable() for land square x,y, across the region's edges */
Int32 land_passable(Int32 x,Int32 y,char monstPass,char doorPass) {
  Int32 rx,ry;

  rx = x;
  ry = y;
  if (land_region(&rx,&ry) == 0) return 0;

  return value_passable(square_value(x,y),monstPass,doorPass);
  }

/* The player has stepped out of region: its view's corner, numbered as region's squares, becomes the same corner
   numbered as the new region's, so the view stays over the same part of the world */
void view_from_region(Int32 region) {
  player.map_x += getPX(region,0) - getPX(player.region,0);
  player.map_y += getPY(region,0) - getPY(player.region,0);
  }

/* The corners the view may take: as far as the world's edges */
void land_view_bounds(Int32* min_x,Int32* max_x,Int32* min_y,Int32* max_y) {
  *min_x = -getPX(player.region,0);
  *max_x = WORLD_WIDTH - TILES_X - getPX(player.region,0);
  *min_y = -getPY(player.region,0);
  *max_y = WORLD_HEIGHT - TILES_Y - getPY(player.region,0);
  }

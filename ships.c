/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The player's ship with the Ship Improvements change: ship_x,ship_y is its square, and ship_r its overworld region
   or, in a town, minus the town's map */

#include "header.h"

#define WATER_TILE  4
#define TOWN_TILE   5
#define SHIP_TILE  12

/* The last time a ship ran into walkable shore: from bump_x,bump_y by bump_dx,bump_dy in bump_region; bump_x -1 if
   the last step was anything else */
static Int32 bump_x = -1;
static Int32 bump_y;
static Int32 bump_dx;
static Int32 bump_dy;
static Int32 bump_region;

/* in_city stays 'Y' on the overworld after leaving a town; in_upland tells them apart */
static Int32 in_town(void) {
  return (in_upland != 'Y' && player.in_city == 'Y');
  }

/* Moves x,y to a water square beside it, the four sides before the corners. Returns 0 when there is none. */
static Int32 water_beside(Int32* x,Int32* y) {
  static const Int32 dx[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
  static const Int32 dy[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
  Int32 i;
  Int32 nx,ny;

  for (i=0; i<8; i++) {
    nx = *x + dx[i];
    ny = *y + dy[i];
    if (nx >= 1 && nx <= 100 && ny >= 1 && ny <= 100 && player.map[nx][ny] == WATER_TILE) {
      *x = nx;
      *y = ny;
      return 1;
      }
    }

  return 0;
  }

/* The town map of the town at x,y on the overworld, or -1 */
static Int32 town_map_at(Int32 x,Int32 y) {
  Int32 i;

  for (i=0; links[i].region < 90; i++)
    if (links[i].typ == 5 && links[i].region == player.region && links[i].x == x && links[i].y == y)
      return links[i].diff;

  return -1;
  }

/* The harbour square of town map city, as x,y. Returns 0 when the town has none. */
Int32 find_harbour(Int32 city,Int32* x,Int32* y) {
  Int32 i;

  for (i=0; tr_ships[i].map < 99; i++)
    if (tr_ships[i].map == city) {
      *x = tr_ships[i].x;
      *y = tr_ships[i].y;
      return 1;
      }

  return 0;
  }

/* The region to record for a ship at the player's square */
Int32 ship_region(void) {
  if (change_ship_improvements == 'Y' && in_town()) return -player.city;

  return player.region;
  }

/* While the player sails, in a town or on the overworld, the ship's record follows them */
void track_ship(void) {
  if (change_ship_improvements != 'Y' || player.inship == ' ') return;
  if (!in_town() && in_upland != 'Y') return;

  player.ship_x = player.x;
  player.ship_y = player.y;
  player.ship_r = ship_region();
  }

/* A ship docked in the town just entered is back at its square */
void place_town_ship(void) {
  if (change_ship_improvements != 'Y' || player.inship != ' ') return;
  if (player.ship_r >= 0 || player.ship_r != -player.city) return;

  player.map[player.ship_x][player.ship_y] = SHIP_TILE;
  }

/* Sailing out of a town puts the ship on the water beside the town's square, not on it */
void ship_leaves_town(void) {
  if (change_ship_improvements != 'Y' || player.inship == ' ') return;
  if (player.map[player.x][player.y] == WATER_TILE) return;

  water_beside(&player.x,&player.y);
  }

/* Whether a ship may sail onto x,y: a town with a harbour, which up_land() then enters */
Int32 sails_into_town(Int32 x,Int32 y) {
  Int32 hx,hy;

  if (change_ship_improvements != 'Y' || player.inship == ' ') return 0;
  if (player.map[x][y] != TOWN_TILE) return 0;

  return find_harbour(town_map_at(x,y),&hx,&hy);
  }

/* A ship sailed into a town is docked in its harbour, the player on it as after docking with E */
void ship_docks_in_town(void) {
  Int32 x,y;

  if (change_ship_improvements != 'Y' || player.inship == ' ') return;
  if (!find_harbour(player.city,&x,&y)) return;

  player.x = x;
  player.y = y;
  player.inship = ' ';
  player.map[x][y] = SHIP_TILE;
  player.ship_x = x;
  player.ship_y = y;
  player.ship_r = -player.city;
  }

/* Walking onto your docked ship boards it, as E does */
void walk_aboard(void) {
  if (change_ship_improvements != 'Y' || player.inship != ' ') return;
  if (player.map[player.x][player.y] != SHIP_TILE) return;

  player.inship = '*';
  player.map[player.x][player.y] = WATER_TILE;
  }

/* A step from x,y by dx,dy in a ship; shore is 1 when it ran into land the player could walk on. The second such step
   in a row, the same way from the same square, docks the ship and returns 1: the player goes ashore. The ship stays
   at x,y on the overworld, and goes to its town's dock in a town. */
Int32 steps_ashore(Int32 x,Int32 y,Int32 dx,Int32 dy,Int32 shore) {
  Int32 again;

  if (change_ship_improvements != 'Y' || player.inship == ' ' || !shore) {
    bump_x = -1;
    return 0;
    }

  again = (bump_x == x && bump_y == y && bump_dx == dx && bump_dy == dy && bump_region == ship_region());
  bump_x = x;
  bump_y = y;
  bump_dx = dx;
  bump_dy = dy;
  bump_region = ship_region();
  if (!again) return 0;

  bump_x = -1;
  if (in_town()) find_harbour(player.city,&x,&y);
  player.inship = ' ';
  player.map[x][y] = SHIP_TILE;
  player.ship_x = x;
  player.ship_y = y;
  player.ship_r = ship_region();

  return 1;
  }

/* Where a ship docks: the player's square, or with the change, water beside it when the player is not on water,
   so a ship never covers a town or an entrance */
void dock_square(Int32* x,Int32* y) {
  if (change_ship_improvements != 'Y' || player.map[*x][*y] == WATER_TILE) return;

  water_beside(x,y);
  }

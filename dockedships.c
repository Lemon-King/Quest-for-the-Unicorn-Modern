/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The player's ship with the Docked Ships change: ship_x,ship_y is its square, and ship_r its overworld region or,
   in a town, minus the town's map */

#include "header.h"

#define SHIP_TILE  12

/* in_city stays 'Y' on the overworld after leaving a town; in_upland tells them apart */
static Int32 in_town(void) {
  return (in_upland != 'Y' && player.in_city == 'Y');
  }

/* The region to record for a ship at the player's square */
Int32 ship_region(void) {
  if (change_docked_ships == 'Y' && in_town()) return -player.city;

  return player.region;
  }

/* While the player sails, in a town or on the overworld, the ship's record follows them */
void track_ship(void) {
  if (change_docked_ships != 'Y' || player.inship == ' ') return;
  if (!in_town() && in_upland != 'Y') return;

  player.ship_x = player.x;
  player.ship_y = player.y;
  player.ship_r = ship_region();
  }

/* A ship docked in the town just entered is back at its square */
void place_town_ship(void) {
  if (change_docked_ships != 'Y' || player.inship != ' ') return;
  if (player.ship_r >= 0 || player.ship_r != -player.city) return;

  player.map[player.ship_x][player.ship_y] = SHIP_TILE;
  }

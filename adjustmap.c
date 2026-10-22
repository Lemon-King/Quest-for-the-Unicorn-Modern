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

#define DEADZONE_PERCENT  20     /* the share of the view, about its middle, the follow camera lets the player cross */

/* The view's middle square along an axis of tiles squares; square 0 is never drawn */
static Int32 view_middle(Int32 tiles) {
  return (tiles + 1) / 2;
  }

/* Moves the view's corner along one axis so the player at stays within the deadzone about its middle, as far as
   min..max allow. Returns 'Y' when it moved. */
static char follow(Int32* corner,Int32 at,Int32 tiles,Int32 min,Int32 max) {
  Int32 reach;
  Int32 wanted;

  reach = tiles * DEADZONE_PERCENT / 200;

  wanted = *corner;
  if (at - wanted < view_middle(tiles) - reach) wanted = at - (view_middle(tiles) - reach);
  if (at - wanted > view_middle(tiles) + reach) wanted = at - (view_middle(tiles) + reach);
  if (wanted < min) wanted = min;
  if (wanted > max) wanted = max;
  if (wanted == *corner) return 'N';

  *corner = wanted;

  return 'Y';
  }

/* The Follow Camera preference works wherever the view scrolls; the arena's view stays on its field */
Int32 camera_follows(void) {
  return (follow_camera == 'Y' && in_arena != 'Y');
  }

/* With the follow camera, centres the view on a player who has just arrived somewhere, as far as the map's edges
   allow: a level, a town, the overworld, or the far end of a teleport */
void camera_arrive(void) {
  if (!camera_follows()) return;

  player.map_x = player.x - view_middle(TILES_X);
  player.map_y = player.y - view_middle(TILES_Y);
  adjust_map();
  }

/* Keeps the view's corner inside the map, squares 1 to 100, or the world's edges on the joined overworld. It scrolls
   when the player comes near its edge, or with the follow camera a square at a time as they leave the deadzone.
   Returns 'Y' when it moved. */
char adjust_map() {
  char  changed;
  Int32 min_x,max_x;
  Int32 min_y,max_y;

  min_x = 0;
  max_x = 100-TILES_X;
  min_y = 0;
  max_y = 100-TILES_Y;
  if (seamless_world()) land_view_bounds(&min_x,&max_x,&min_y,&max_y);

  if (camera_follows()) {
    changed = follow(&player.map_x,player.x,TILES_X,min_x,max_x);
    if (follow(&player.map_y,player.y,TILES_Y,min_y,max_y) == 'Y') changed = 'Y';
    return changed;
    }

  changed = 'N';
  if (player.map_x > max_x) {
    player.map_x = max_x;
    changed = 'Y';
    }
  if (player.map_y > max_y) {
    player.map_y = max_y;
    changed = 'Y';
    }
  if (player.map_x < min_x) {
    player.map_x = min_x;
    changed = 'Y';
    }
  if (player.map_y < min_y) {
    player.map_y = min_y;
    changed = 'Y';
    }
  if (player.y - player.map_y < 2) {
    player.map_y = player.y - (TILES_Y-4);
    changed = 'Y';
    if (player.map_y < min_y) player.map_y = min_y;
    }
  if (player.y - player.map_y > TILES_Y-2) {
    player.map_y = player.y - 4;
    changed = 'Y';
    if (player.map_y > max_y) player.map_y = max_y;
    }
  if (player.x - player.map_x < 2) {
    player.map_x = player.x - (TILES_X-4);
    changed = 'Y';
    if (player.map_x < min_x) player.map_x = min_x;
    }
  if (player.x - player.map_x > TILES_X-2) {
    player.map_x = player.x - 4;
    changed = 'Y';
    if (player.map_x > max_x) player.map_x = max_x;
    }
  return changed;
  }

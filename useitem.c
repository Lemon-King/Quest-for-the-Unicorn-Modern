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

void use_item() {
  OBJECT_REC  it;
  OBJECT_REC* item;
  Int32 ground_x,ground_y;
  item = NULL;
  if (player.map[player.x][player.y] >= 1000 &&
      player.map[player.x][player.y] < 2000) {
    it = player.map_objects[player.map[player.x][player.y] - 1000];
    player.map_objects[player.map[player.x][player.y] - 1000].otype = O_NONE;
    item = &it;
    player.map[player.x][player.y] = currentFloor;
    show_spot(player.x,player.y);
    }
  saveScreen();
  item = inventory(item);

  /* Where inventory() put an item used from the ground; perform_effect() below can call inventory() again */
  ground_x = drop_x;
  ground_y = drop_y;
  restoreScreen();
  show_spot(player.x,player.y);
  show_player();
  if (item != NULL && item->otype != O_NONE) {
    takes_time = 'Y';
    if (item->otype == O_FOOD && player.health > 100) {
      msg("Too full to eat any more",10);
      }
    else {
      if (item->charges > 0) item->charges--;
      perform_effect(item);
      if (item->charges == 0) {
        item->otype = O_NONE;

        /* Clears the square the item lay on. It used to clear the player's own square, which turned stairs or a door
           to floor when the item had been dropped beside them, and left the item's square pointing at an empty slot */
        if (use_from_ground == 'Y' && ground_x > 0 && ground_y > 0) {
          player.map[ground_x][ground_y] = currentFloor;
          show_spot(ground_x,ground_y);
          }
        }
      }
    }
  graphmode();
  if (clair == ' ') show_map();
  show_sm_map();
  discover();
  show_side();
  show_player();
  show_messages();
  clair = ' ';
  } /* void use_item() */


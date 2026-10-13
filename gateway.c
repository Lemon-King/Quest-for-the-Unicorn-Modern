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

/* The Gateway as the current dungeon, on entering it from the overworld or returning to it by Sun Stone */
void set_gateway_dungeon() {
  strcpy(player.d_name,"The Gateway");
  player.diff = 20;
  if (player.gateway != 'F') player.gateway = '*';
  player.dungeon_number = GATEWAY_DUNGEON;
  }

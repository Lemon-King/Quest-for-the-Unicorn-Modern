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

#define PLAYER_SPRITE  35

void check_mount() {
  Int32 i;
  player.mount = 0; player.peg = 0;
  for (i=1; i<=5; i++)
    if (player.party[i].hp > 0) {
      if ((player.party[i].special & MS_FLY) != 0 &&
          (player.party[i].special & MS_MOUNT) != 0) player.peg = 1;
      if ((player.party[i].special & MS_MOUNT) != 0) player.mount = 1;
      }
  }

/* The sprite of the mount you ride, the first that flies when you fly, or 0 when none is in the party */
static Int32 mount_sprite(void) {
  UInt32 ridden;
  Int32  i;

  ridden = (player.peg != 0) ? (MS_MOUNT | MS_FLY) : MS_MOUNT;
  for (i=1; i<=5; i++)
    if (player.party[i].hp > 0 && (player.party[i].special & ridden) == ridden) return player.party[i].sprite;

  return 0;
  }

/* Your sprite on the overworld: with the Show Mount preference, the mount you ride */
Int32 rider_sprite(void) {
  Int32 sprite;

  sprite = (show_mount == 'Y' && player.mount != 0) ? mount_sprite() : 0;

  return (sprite != 0) ? sprite : PLAYER_SPRITE;
  }


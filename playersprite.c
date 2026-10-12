/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The sprite that stands for the player: the class's own with the Unique Class Sprites preference, and on the
   overworld the mount ridden with the Show Mount preference. */

#include "header.h"

#define PLAYER_SPRITE  35

/* The sprite of the mount you ride, the first that flies when you fly, or 0 when none is in the party */
static Int32 mount_sprite(void) {
  UInt32 ridden;
  Int32  i;

  ridden = (player.peg != 0) ? (MS_MOUNT | MS_FLY) : MS_MOUNT;
  for (i=1; i<=5; i++)
    if (player.party[i].hp > 0 && (player.party[i].special & ridden) == ridden) return player.party[i].sprite;

  return 0;
  }

Int32 player_sprite(void) {
  if (class_sprites == 'Y') return classes[player.class-1].sprite;

  return PLAYER_SPRITE;
  }

/* Your sprite on the overworld, where with the Show Mount preference the mount you ride stands for you */
Int32 rider_sprite(void) {
  Int32 sprite;

  sprite = (show_mount == 'Y' && player.mount != 0) ? mount_sprite() : 0;

  return (sprite != 0) ? sprite : player_sprite();
  }

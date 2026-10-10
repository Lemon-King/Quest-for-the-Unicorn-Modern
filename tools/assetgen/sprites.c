/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The unicorn sprite for the application icon, straight from the game's sprite file. C, because the XPM arrays are
   static char* initialised from string literals, which C++ rejects. */

#include "unicorn.xpm"

const char* const* unicorn_sprite(void) {
  return (const char* const*)sprite_132;
  }

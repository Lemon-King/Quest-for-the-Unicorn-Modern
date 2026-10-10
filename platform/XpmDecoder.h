/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#pragma once

#include "Image.h"

namespace platform {

/* Decodes the game's compiled-in XPM sprites by the rules graphics.c drew them with: one character per pixel, colours
   as "#" with 12 hex digits (each 16-bit channel above 255 divided by 256), and "None" for transparent */
class XpmDecoder {
  public:
    Image decode(const char* const* xpm) const;
  };

}

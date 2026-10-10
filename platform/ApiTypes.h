/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* What the opaque handles of platform.h are on the C++ side */

#pragma once

#include "Canvas.h"
#include "GameHost.h"
#include "GlyphFont.h"
#include "SpriteCache.h"
#include "platform.h"

struct CANVAS {
  platform::Canvas canvas;
  };

struct GLYPH_FONT {
  platform::GlyphFont font;
  };

struct SPRITE_CACHE {
  platform::SpriteCache cache;
  };

struct GAME_HOST {
  platform::GameHost& host;
  };

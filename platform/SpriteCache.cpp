/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "SpriteCache.h"

namespace platform {

const Image& SpriteCache::sprite(const char* const* xpm) {
  auto found = images_.find(xpm);
  if (found != images_.end()) return found->second;
  return images_.emplace(xpm, decoder_.decode(xpm)).first->second;
  }

}

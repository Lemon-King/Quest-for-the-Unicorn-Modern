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

#include <unordered_map>

#include "Image.h"
#include "XpmDecoder.h"

namespace platform {

/* Each compiled-in sprite decoded once. The XPM arrays are static, so their addresses are stable keys */
class SpriteCache {
  public:
    const Image& sprite(const char* const* xpm);

  private:
    XpmDecoder                                    decoder_;
    std::unordered_map<const char* const*, Image> images_;
  };

}

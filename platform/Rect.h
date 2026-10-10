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

#include <cstdint>

namespace platform {

/* An area of pixels; right and bottom are exclusive */
struct Rect {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;

  bool empty() const { return right <= left || bottom <= top; }
  int32_t width() const { return right - left; }
  int32_t height() const { return bottom - top; }
  Rect unite(const Rect& other) const;
  Rect intersect(const Rect& other) const;
  };

}

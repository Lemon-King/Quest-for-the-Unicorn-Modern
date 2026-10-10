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
#include <cstdio>
#include <string>

#include "GdiSurface.h"

/* "" when the canvas pixels equal the surface's in the area, else where and how they first differ */
inline std::string comparePixels(const uint32_t* canvas, int32_t canvasWidth, const GdiSurface& surface,
                                 int32_t left, int32_t top, int32_t right, int32_t bottom) {
  const uint32_t* expected = surface.pixels();
  for (int32_t y = top; y < bottom; y++)
    for (int32_t x = left; x < right; x++) {
      uint32_t want = expected[static_cast<size_t>(y) * static_cast<size_t>(surface.width()) + static_cast<size_t>(x)]
                      & 0x00ffffff;
      uint32_t got = canvas[static_cast<size_t>(y) * static_cast<size_t>(canvasWidth) + static_cast<size_t>(x)];
      if (want != got) {
        char detail[96];
        std::snprintf(detail, sizeof(detail), "at %d,%d GDI #%06x, canvas #%06x", static_cast<int>(x),
                      static_cast<int>(y), static_cast<unsigned>(want), static_cast<unsigned>(got));
        return detail;
        }
      }
  return "";
  }

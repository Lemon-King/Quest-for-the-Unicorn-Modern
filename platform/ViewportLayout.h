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

/* Where the scaled canvas sits in the client area */
struct Placement {
  int32_t left;
  int32_t top;
  int32_t width;
  int32_t height;
  };

/* Fits the canvas to a client area at its own aspect ratio, centred on black: as large as fits (never below 1:1), or
   with integerScaling the largest whole-number scale that fits */
class ViewportLayout {
  public:
    ViewportLayout(int32_t canvasWidth, int32_t canvasHeight, bool integerScaling);
    Placement place(int32_t clientWidth, int32_t clientHeight) const;

  private:
    int32_t canvasWidth_;
    int32_t canvasHeight_;
    bool    integerScaling_;
  };

}

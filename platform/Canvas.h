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
#include <vector>

#include "Rect.h"

namespace platform {

class Image;

/* The game's back buffer: 0x00RRGGBB pixels, and the area drawn since the dirty area was last taken */
class Canvas {
  public:
    Canvas(int32_t width, int32_t height);

    int32_t width() const;
    int32_t height() const;
    const uint32_t* pixels() const;

    void fill(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb);
    void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb);
    void point(int32_t x, int32_t y, uint32_t rgb);
    void mask(int32_t x, int32_t y, const uint8_t* rows, int32_t height, uint32_t rgb);
    void image(int32_t x, int32_t y, const Image& image);
    void copyFrom(const Canvas& source);

    Rect takeDirty();

  private:
    void plot(int32_t x, int32_t y, uint32_t rgb);
    void touch(const Rect& area);

    int32_t               width_;
    int32_t               height_;
    std::vector<uint32_t> pixels_;
    Rect                  dirty_;
  };

}

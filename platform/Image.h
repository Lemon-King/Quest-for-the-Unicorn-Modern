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

namespace platform {

/* Pixels of a decoded sprite, 0x00RRGGBB, with the transparent ones flagged */
class Image {
  public:
    Image(int32_t width, int32_t height, std::vector<uint32_t> pixels, std::vector<bool> opaque);

    int32_t width() const;
    int32_t height() const;
    uint32_t pixel(int32_t x, int32_t y) const;
    bool opaque(int32_t x, int32_t y) const;

    /* Half the size, rounded up: each pixel averages the opaque pixels of its 2x2 block, and is transparent when fewer
       than half of the block's pixels are opaque. Blocks on an odd edge hold two pixels or one. */
    Image halved() const;

  private:
    struct Block {
      uint32_t rgb;
      bool     opaque;
      };

    Block averageBlock(int32_t left, int32_t top) const;

    int32_t               width_;
    int32_t               height_;
    std::vector<uint32_t> pixels_;
    std::vector<bool>     opaque_;
  };

}

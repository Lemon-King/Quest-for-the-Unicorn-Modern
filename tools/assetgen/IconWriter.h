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
#include <filesystem>
#include <vector>

#include "Image.h"

/* An .ico holding one sprite exactly as the game draws it: its pixels unscaled, centred on a transparent square */
class IconWriter {
  public:
    IconWriter(const platform::Image& sprite, int32_t size);

    void write(const std::filesystem::path& file) const;

  private:
    std::vector<uint32_t> pixels() const;      /* 0xAARRGGBB, top row first */

    const platform::Image& sprite_;
    int32_t                size_;
  };

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "Image.h"

#include <stdexcept>
#include <utility>

namespace platform {

Image::Image(int32_t width, int32_t height, std::vector<uint32_t> pixels, std::vector<bool> opaque)
  : width_(width), height_(height), pixels_(std::move(pixels)), opaque_(std::move(opaque)) {
  if (width < 1 || height < 1) throw std::invalid_argument("image size must be positive");
  size_t count = static_cast<size_t>(width) * static_cast<size_t>(height);
  if (pixels_.size() != count || opaque_.size() != count) throw std::invalid_argument("image data does not fit its size");
  }

int32_t Image::width() const {
  return width_;
  }

int32_t Image::height() const {
  return height_;
  }

uint32_t Image::pixel(int32_t x, int32_t y) const {
  return pixels_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)];
  }

bool Image::opaque(int32_t x, int32_t y) const {
  return opaque_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)];
  }

}

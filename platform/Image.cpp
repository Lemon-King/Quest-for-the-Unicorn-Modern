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

#include <algorithm>
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

Image Image::halved() const {
  int32_t               width = (width_ + 1) / 2;
  int32_t               height = (height_ + 1) / 2;
  size_t                count = static_cast<size_t>(width) * static_cast<size_t>(height);
  std::vector<uint32_t> pixels;
  std::vector<bool>     opaque;

  pixels.reserve(count);
  opaque.reserve(count);

  for (int32_t y = 0; y < height; y++)
    for (int32_t x = 0; x < width; x++) {
      Block block = averageBlock(2 * x, 2 * y);
      pixels.push_back(block.rgb);
      opaque.push_back(block.opaque);
      }

  return Image(width, height, std::move(pixels), std::move(opaque));
  }

Image::Block Image::averageBlock(int32_t left, int32_t top) const {
  int32_t  right = std::min(left + 2, width_);
  int32_t  bottom = std::min(top + 2, height_);
  int32_t  area = (right - left) * (bottom - top);
  uint32_t covered = 0;
  uint32_t red = 0;
  uint32_t green = 0;
  uint32_t blue = 0;

  for (int32_t y = top; y < bottom; y++)
    for (int32_t x = left; x < right; x++) {
      if (!opaque(x, y)) continue;

      uint32_t rgb = pixel(x, y);
      red += (rgb >> 16) & 0xff;
      green += (rgb >> 8) & 0xff;
      blue += rgb & 0xff;
      covered++;
      }

  if (covered == 0 || 2 * static_cast<int32_t>(covered) < area) return Block{0, false};

  red = (red + covered / 2) / covered;
  green = (green + covered / 2) / covered;
  blue = (blue + covered / 2) / covered;

  return Block{(red << 16) | (green << 8) | blue, true};
  }

}

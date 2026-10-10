/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "FrameMailbox.h"

#include <algorithm>
#include <stdexcept>

namespace platform {

/* Black, and all of it still to upload */
FrameMailbox::FrameMailbox(int32_t width, int32_t height)
  : mutex_(), width_(width), height_(height), pixels_(), changed_{0, 0, width, height} {
  if (width < 1 || height < 1) throw std::invalid_argument("frame size must be positive");
  pixels_.assign(static_cast<size_t>(width) * static_cast<size_t>(height), 0);
  }

/* Copies the rows the canvas changed since its last publish; false when it changed nothing */
bool FrameMailbox::publish(Canvas& canvas) {
  if (canvas.width() != width_ || canvas.height() != height_) throw std::invalid_argument("canvas and frame sizes differ");
  Rect area = canvas.takeDirty();
  if (area.empty()) return false;

  std::lock_guard<std::mutex> lock(mutex_);
  for (int32_t row = area.top; row < area.bottom; row++) {
    size_t start = static_cast<size_t>(row) * static_cast<size_t>(width_) + static_cast<size_t>(area.left);
    std::copy_n(canvas.pixels() + start, area.width(), pixels_.begin() + static_cast<ptrdiff_t>(start));
    }
  changed_ = changed_.unite(area);
  return true;
  }

/* Hands the frame and its changed area to upload, if anything changed, and starts the next area afresh */
void FrameMailbox::collect(const std::function<void(const uint32_t* pixels, int32_t stride, const Rect& area)>& upload) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (changed_.empty()) return;
  upload(pixels_.data(), width_, changed_);
  changed_ = Rect{0, 0, 0, 0};
  }

/* The whole frame is uploaded again, as a new device needs */
void FrameMailbox::invalidate() {
  std::lock_guard<std::mutex> lock(mutex_);
  changed_ = Rect{0, 0, width_, height_};
  }

}

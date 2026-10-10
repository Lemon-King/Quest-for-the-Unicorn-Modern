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
#include <functional>
#include <mutex>
#include <vector>

#include "Canvas.h"
#include "Rect.h"

namespace platform {

/* The newest frame on its way to the presenter: the canvas pixels as last published, and the area that changed since
   the presenter last took them */
class FrameMailbox {
  public:
    FrameMailbox(int32_t width, int32_t height);

    bool publish(Canvas& canvas);
    void collect(const std::function<void(const uint32_t* pixels, int32_t stride, const Rect& area)>& upload);
    void invalidate();

  private:
    std::mutex            mutex_;
    int32_t               width_;
    int32_t               height_;
    std::vector<uint32_t> pixels_;
    Rect                  changed_;
  };

}

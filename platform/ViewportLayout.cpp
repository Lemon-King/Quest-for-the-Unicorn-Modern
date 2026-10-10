/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "ViewportLayout.h"

#include <algorithm>
#include <stdexcept>

namespace platform {

ViewportLayout::ViewportLayout(int32_t canvasWidth, int32_t canvasHeight, bool integerScaling)
  : canvasWidth_(canvasWidth), canvasHeight_(canvasHeight), integerScaling_(integerScaling) {
  if (canvasWidth < 1 || canvasHeight < 1) throw std::invalid_argument("canvas size must be positive");
  }

/* A client area smaller than the canvas shows it at 1:1, centred and cropped. At a whole-number scale the shader's
   blend weights are all 0, so integer scaling is plain nearest-neighbour. */
Placement ViewportLayout::place(int32_t clientWidth, int32_t clientHeight) const {
  int32_t width = canvasWidth_;
  int32_t height = canvasHeight_;

  if (integerScaling_) {
    int32_t scale = std::max(1, std::min(clientWidth / canvasWidth_, clientHeight / canvasHeight_));
    return Placement{(clientWidth - canvasWidth_ * scale) / 2, (clientHeight - canvasHeight_ * scale) / 2,
                     canvasWidth_ * scale, canvasHeight_ * scale};
    }

  if (clientWidth >= canvasWidth_ && clientHeight >= canvasHeight_) {
    if (clientWidth * canvasHeight_ <= clientHeight * canvasWidth_) {
      width = clientWidth;
      height = clientWidth * canvasHeight_ / canvasWidth_;
      }
    else {
      width = clientHeight * canvasWidth_ / canvasHeight_;
      height = clientHeight;
      }
    }

  return Placement{(clientWidth - width) / 2, (clientHeight - height) / 2, width, height};
  }

}

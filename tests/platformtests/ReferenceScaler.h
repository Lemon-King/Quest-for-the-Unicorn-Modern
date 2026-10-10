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

/* graphics.c's CPU viewport (_rcs_layoutViewport, _rcs_sampleAxis, _rcs_blendPixel, _rcs_samplePixel and
   _rcs_scaleImage), kept as the oracle for the Direct3D scaler: the window pixels the GDI build showed for a canvas */
class ReferenceScaler {
  public:
    ReferenceScaler(const uint32_t* canvas, int32_t width, int32_t height);

    /* The client area's pixels, 0x00RRGGBB row by row: the scaled canvas centred on black */
    std::vector<uint32_t> window(int32_t clientWidth, int32_t clientHeight) const;

  private:
    /* One axis of the canvas: its size in canvas pixels and in window pixels */
    struct Axis {
      int32_t size;
      int32_t scaled;
      };

    /* The canvas pixel a window pixel starts in and, out of 256, how much of it lies over the next one */
    struct Sample {
      int32_t source;
      int32_t blend;
      };

    static Sample sampleAxis(Axis axis, int32_t i);
    static uint32_t blendPixel(uint32_t first, uint32_t second, int32_t weight);
    static uint32_t samplePixel(const uint32_t* row, Sample column);

    const uint32_t* canvas_;
    int32_t         width_;
    int32_t         height_;
  };

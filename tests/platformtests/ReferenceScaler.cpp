/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "ReferenceScaler.h"

ReferenceScaler::ReferenceScaler(const uint32_t* canvas, int32_t width, int32_t height)
  : canvas_(canvas), width_(width), height_(height) {
  }

/* Window pixel i covers canvas span i*size/scaled .. (i+1)*size/scaled: at most two canvas pixels */
ReferenceScaler::Sample ReferenceScaler::sampleAxis(Axis axis, int32_t i) {
  Sample  sample;
  int32_t overlap;
  sample.source = i * axis.size / axis.scaled;
  overlap = (i + 1) * axis.size - (sample.source + 1) * axis.scaled;
  sample.blend = (overlap > 0) ? overlap * 256 / axis.size : 0;
  return sample;
  }

/* Mixes weight/256 of second into first, one channel at a time */
uint32_t ReferenceScaler::blendPixel(uint32_t first, uint32_t second, int32_t weight) {
  uint32_t keep = static_cast<uint32_t>(256 - weight);
  uint32_t take = static_cast<uint32_t>(weight);
  uint32_t redBlue = ((first & 0xff00ff) * keep + (second & 0xff00ff) * take) >> 8;
  uint32_t green = ((first & 0x00ff00) * keep + (second & 0x00ff00) * take) >> 8;
  return (redBlue & 0xff00ff) | (green & 0x00ff00);
  }

uint32_t ReferenceScaler::samplePixel(const uint32_t* row, Sample column) {
  if (column.blend == 0) return row[column.source];
  return blendPixel(row[column.source], row[column.source + 1], column.blend);
  }

/* _rcs_layoutViewport places the canvas, never below 1:1; _rcs_paintViewport frames it in black and fills it with
   _rcs_scaleImage's samples. At 1:1 every sample is one canvas pixel, the plain copy the GDI build made. */
std::vector<uint32_t> ReferenceScaler::window(int32_t clientWidth, int32_t clientHeight) const {
  Axis columns{width_, width_};
  Axis rows{height_, height_};
  if (clientWidth < width_ || clientHeight < height_) {
    /* 1:1, centred and cropped */
    }
  else if (clientWidth * height_ <= clientHeight * width_) {
    columns.scaled = clientWidth;
    rows.scaled = clientWidth * height_ / width_;
    }
  else {
    columns.scaled = clientHeight * width_ / height_;
    rows.scaled = clientHeight;
    }
  int32_t left = (clientWidth - columns.scaled) / 2;
  int32_t top = (clientHeight - rows.scaled) / 2;

  std::vector<uint32_t> pixels(static_cast<size_t>(clientWidth) * static_cast<size_t>(clientHeight), 0);
  for (int32_t y = 0; y < rows.scaled; y++) {
    if (top + y < 0 || top + y >= clientHeight) continue;
    Sample          row = sampleAxis(rows, y);
    const uint32_t* upper = canvas_ + static_cast<size_t>(row.source) * static_cast<size_t>(width_);
    const uint32_t* lower = upper + width_;

    for (int32_t x = 0; x < columns.scaled; x++) {
      if (left + x < 0 || left + x >= clientWidth) continue;
      Sample   column = sampleAxis(columns, x);
      uint32_t color = (row.blend == 0) ? samplePixel(upper, column)
                                        : blendPixel(samplePixel(upper, column), samplePixel(lower, column), row.blend);
      pixels[static_cast<size_t>(top + y) * static_cast<size_t>(clientWidth) + static_cast<size_t>(left + x)] = color;
      }
    }
  return pixels;
  }

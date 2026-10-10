/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "Canvas.h"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

#include "Image.h"

namespace platform {

namespace {

int32_t floorDivide(int32_t numerator, int32_t denominator) {
  int32_t quotient = numerator / denominator;
  if (numerator % denominator != 0 && (numerator < 0) != (denominator < 0)) quotient--;
  return quotient;
  }

}

Rect Rect::unite(const Rect& other) const {
  if (empty()) return other;
  if (other.empty()) return *this;
  return Rect{std::min(left, other.left), std::min(top, other.top), std::max(right, other.right),
              std::max(bottom, other.bottom)};
  }

Rect Rect::intersect(const Rect& other) const {
  Rect area{std::max(left, other.left), std::max(top, other.top), std::min(right, other.right),
            std::min(bottom, other.bottom)};
  if (area.empty()) return Rect{0, 0, 0, 0};
  return area;
  }

Canvas::Canvas(int32_t width, int32_t height)
  : width_(width), height_(height), pixels_(), dirty_{0, 0, 0, 0} {
  if (width < 1 || height < 1) throw std::invalid_argument("canvas size must be positive");
  pixels_.assign(static_cast<size_t>(width) * static_cast<size_t>(height), 0);
  }

int32_t Canvas::width() const {
  return width_;
  }

int32_t Canvas::height() const {
  return height_;
  }

const uint32_t* Canvas::pixels() const {
  return pixels_.data();
  }

/* As GDI's FillRect: right and bottom are exclusive, and a negative size reaches back from (x,y) */
void Canvas::fill(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb) {
  Rect area = Rect{std::min(x, x + width), std::min(y, y + height), std::max(x, x + width), std::max(y, y + height)}
                .intersect(Rect{0, 0, width_, height_});
  if (area.empty()) return;

  for (int32_t row = area.top; row < area.bottom; row++)
    std::fill_n(pixels_.begin() + static_cast<ptrdiff_t>(row) * width_ + area.left, area.width(), rgb);

  touch(area);
  }

/* As GDI's MoveToEx and LineTo with a one-pixel pen: the end point is left out, and a minor coordinate exactly half
   way between two pixels rounds down */
void Canvas::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb) {
  int32_t dx = x2 - x1;
  int32_t dy = y2 - y1;
  int32_t steps = std::max(std::abs(dx), std::abs(dy));
  bool    xMajor = std::abs(dx) >= std::abs(dy);

  for (int32_t i = 0; i < steps; i++) {
    if (xMajor) plot(x1 + (dx > 0 ? i : -i), y1 + floorDivide(2 * i * dy + steps - 1, 2 * steps), rgb);
    else        plot(x1 + floorDivide(2 * i * dx + steps - 1, 2 * steps), y1 + (dy > 0 ? i : -i), rgb);
    }

  touch(Rect{std::min(x1, x2), std::min(y1, y2), std::max(x1, x2) + 1, std::max(y1, y2) + 1});
  }

void Canvas::point(int32_t x, int32_t y, uint32_t rgb) {
  plot(x, y, rgb);
  touch(Rect{x, y, x + 1, y + 1});
  }

/* One glyph row per byte, leftmost pixel in the top bit */
void Canvas::mask(int32_t x, int32_t y, const uint8_t* rows, int32_t height, uint32_t rgb) {
  for (int32_t row = 0; row < height; row++)
    for (int32_t column = 0; column < 8; column++)
      if (rows[row] & (0x80 >> column)) plot(x + column, y + row, rgb);

  touch(Rect{x, y, x + 8, y + height});
  }

/* Only the opaque pixels are drawn, as rcs_drawPixmap skipped the "None" colour */
void Canvas::image(int32_t x, int32_t y, const Image& image) {
  for (int32_t row = 0; row < image.height(); row++)
    for (int32_t column = 0; column < image.width(); column++)
      if (image.opaque(column, row)) plot(x + column, y + row, image.pixel(column, row));

  touch(Rect{x, y, x + image.width(), y + image.height()});
  }

void Canvas::copyFrom(const Canvas& source) {
  if (source.width_ != width_ || source.height_ != height_) throw std::invalid_argument("canvas sizes differ");
  /* In place: the viewport holds pixels() for the canvas's lifetime, so the buffer must never move */
  std::copy(source.pixels_.begin(), source.pixels_.end(), pixels_.begin());
  touch(Rect{0, 0, width_, height_});
  }

Rect Canvas::takeDirty() {
  Rect area = dirty_;
  dirty_ = Rect{0, 0, 0, 0};
  return area;
  }

void Canvas::plot(int32_t x, int32_t y, uint32_t rgb) {
  if (x < 0 || y < 0 || x >= width_ || y >= height_) return;
  pixels_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)] = rgb;
  }

void Canvas::touch(const Rect& area) {
  dirty_ = dirty_.unite(area.intersect(Rect{0, 0, width_, height_}));
  }

}

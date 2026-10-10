/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "GdiSurface.h"

#include <cstring>
#include <stdexcept>

GdiSurface::GdiSurface(int32_t width, int32_t height)
  : width_(width), height_(height), dc_(nullptr), section_(nullptr), previousBitmap_(nullptr), bits_(nullptr),
    font_(nullptr) {
  BITMAPINFO info{};
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = width;
  info.bmiHeader.biHeight = -height;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 32;
  info.bmiHeader.biCompression = BI_RGB;

  void* bits = nullptr;
  dc_ = CreateCompatibleDC(nullptr);
  section_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
  /* The font exactly as graphics.c created it */
  font_ = CreateFontA(12, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                      NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Terminal");
  if (dc_ == nullptr || section_ == nullptr || bits == nullptr || font_ == nullptr) {
    if (font_ != nullptr) DeleteObject(font_);
    if (section_ != nullptr) DeleteObject(section_);
    if (dc_ != nullptr) DeleteDC(dc_);
    throw std::runtime_error("cannot create a GDI surface");
    }

  bits_ = static_cast<uint32_t*>(bits);
  previousBitmap_ = SelectObject(dc_, section_);
  clear();
  }

GdiSurface::~GdiSurface() {
  SelectObject(dc_, previousBitmap_);
  DeleteObject(font_);
  DeleteObject(section_);
  DeleteDC(dc_);
  }

int32_t GdiSurface::width() const {
  return width_;
  }

int32_t GdiSurface::height() const {
  return height_;
  }

HDC GdiSurface::dc() const {
  return dc_;
  }

COLORREF GdiSurface::colorRef(uint32_t rgb) {
  return RGB((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
  }

void GdiSurface::clear() {
  GdiFlush();
  std::memset(bits_, 0, sizeof(uint32_t) * static_cast<size_t>(width_) * static_cast<size_t>(height_));
  }

/* rcs_drawFilledBox */
void GdiSurface::fill(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb) {
  RECT   rect{x, y, x + width, y + height};
  HBRUSH brush = CreateSolidBrush(colorRef(rgb));
  FillRect(dc_, &rect, brush);
  DeleteObject(brush);
  }

/* rcs_drawLine */
void GdiSurface::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb) {
  HPEN    pen = CreatePen(PS_SOLID, 0, colorRef(rgb));
  HGDIOBJ previous = SelectObject(dc_, pen);
  MoveToEx(dc_, x1, y1, nullptr);
  LineTo(dc_, x2, y2);
  SelectObject(dc_, previous);
  DeleteObject(pen);
  }

/* rcs_drawPoint */
void GdiSurface::point(int32_t x, int32_t y, uint32_t rgb) {
  line(x, y, x + 1, y, rgb);
  }

/* rcs_drawString: one TextOut per character, 8 pixels apart, on the baseline, transparent */
void GdiSurface::text(int32_t x, int32_t baseline, const char* text, uint32_t rgb) {
  HGDIOBJ previous = SelectObject(dc_, font_);
  SetTextAlign(dc_, TA_LEFT | TA_BASELINE);
  SetBkColor(dc_, RGB(255, 255, 255));
  SetTextColor(dc_, colorRef(rgb));
  SetBkMode(dc_, TRANSPARENT);
  for (int32_t i = 0; text[i] != 0; i++) TextOutA(dc_, x + i * 8, baseline, &text[i], 1);
  SelectObject(dc_, previous);
  }

uint32_t GdiSurface::pixel(int32_t x, int32_t y) const {
  GdiFlush();
  return bits_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)] & 0x00ffffff;
  }

/* The raw rows, top first; the top byte of each pixel is not part of its colour */
const uint32_t* GdiSurface::pixels() const {
  GdiFlush();
  return bits_;
  }

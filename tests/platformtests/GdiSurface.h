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

#include <windows.h>

#include <cstdint>

/* The oracle: a top-down 32-bit DIB drawn with exactly the GDI calls the deleted graphics.c made */
class GdiSurface {
  public:
    GdiSurface(int32_t width, int32_t height);
    ~GdiSurface();

    GdiSurface(const GdiSurface&) = delete;
    GdiSurface& operator=(const GdiSurface&) = delete;

    int32_t width() const;
    int32_t height() const;
    HDC dc() const;

    void clear();
    void fill(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t rgb);
    void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t rgb);
    void point(int32_t x, int32_t y, uint32_t rgb);
    void text(int32_t x, int32_t baseline, const char* text, uint32_t rgb);

    /* 0x00RRGGBB; pending GDI work is flushed first */
    uint32_t pixel(int32_t x, int32_t y) const;
    const uint32_t* pixels() const;

    static COLORREF colorRef(uint32_t rgb);

  private:
    int32_t   width_;
    int32_t   height_;
    HDC       dc_;
    HBITMAP   section_;
    HGDIOBJ   previousBitmap_;
    uint32_t* bits_;
    HFONT     font_;
  };

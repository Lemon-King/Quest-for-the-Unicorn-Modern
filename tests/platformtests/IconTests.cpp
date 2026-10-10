/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The application icon is the game's unicorn sprite and nothing else: its pixels unscaled at (3,2), all else clear */

#include <windows.h>

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include "TestRunner.h"
#include "XpmDecoder.h"

/* selectsprite.c, compiled into this program as C */
extern "C" char** select_sprite(int32_t n);

namespace {

constexpr int32_t ICON_SIZE = 32;
constexpr int32_t SPRITE_LEFT = 3;
constexpr int32_t SPRITE_TOP = 2;

/* A bitmap read as 32-bit top-down pixels, 0x00RRGGBB; a monochrome mask reads as 0 or 0xffffff */
std::vector<uint32_t> readBitmap(HBITMAP bitmap) {
  std::vector<uint32_t> pixels(static_cast<size_t>(ICON_SIZE) * ICON_SIZE, 0);
  BITMAPINFO            info = {};
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = ICON_SIZE;
  info.bmiHeader.biHeight = -ICON_SIZE;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 32;
  info.bmiHeader.biCompression = BI_RGB;

  HDC dc = GetDC(nullptr);
  int lines = GetDIBits(dc, bitmap, 0, ICON_SIZE, pixels.data(), &info, DIB_RGB_COLORS);
  ReleaseDC(nullptr, dc);
  if (lines != ICON_SIZE) pixels.clear();
  return pixels;
  }

void iconMatchesSprite(TestRunner& runner, const std::string& root) {
  if (root.empty()) {
    runner.check(false, "IconMatchesSprite", "needs the repo root as its first argument, to find res\\unicorn.ico");
    return;
    }

  std::string file = root + "\\res\\unicorn.ico";
  HICON       icon = static_cast<HICON>(LoadImageA(nullptr, file.c_str(), IMAGE_ICON, ICON_SIZE, ICON_SIZE,
                                                   LR_LOADFROMFILE));
  ICONINFO    parts = {};
  if (icon == nullptr || !GetIconInfo(icon, &parts)) {
    runner.check(false, "IconMatchesSprite", "could not load " + file);
    if (icon != nullptr) DestroyIcon(icon);
    return;
    }
  std::vector<uint32_t> color = readBitmap(parts.hbmColor);
  std::vector<uint32_t> mask = readBitmap(parts.hbmMask);
  DeleteObject(parts.hbmColor);
  DeleteObject(parts.hbmMask);
  DestroyIcon(icon);
  if (color.empty() || mask.empty()) {
    runner.check(false, "IconMatchesSprite", "could not read the icon's bitmaps");
    return;
    }

  try {
    platform::Image sprite = platform::XpmDecoder().decode(const_cast<const char* const*>(select_sprite(132)));
    std::string     first;
    int32_t         differ = 0;
    for (int32_t y = 0; y < ICON_SIZE; y++)
      for (int32_t x = 0; x < ICON_SIZE; x++) {
        size_t  i = static_cast<size_t>(y) * ICON_SIZE + static_cast<size_t>(x);
        int32_t sx = x - SPRITE_LEFT;
        int32_t sy = y - SPRITE_TOP;
        bool    inside = sx >= 0 && sy >= 0 && sx < sprite.width() && sy < sprite.height();
        bool    opaque = inside && sprite.opaque(sx, sy);
        bool    clear = (mask[i] & 0xffffff) != 0;
        bool    same = opaque ? (!clear && (color[i] & 0xffffff) == sprite.pixel(sx, sy)) : clear;
        if (same) continue;
        if (differ++ == 0)
          first = "pixel " + std::to_string(x) + "," + std::to_string(y) + (opaque ? " should be the sprite's colour" :
                                                                             " should be transparent");
        }
    runner.check(differ == 0, "IconMatchesSprite", std::to_string(differ) + " pixels differ, first " + first);
    }
  catch (const std::exception& error) {
    runner.check(false, "IconMatchesSprite", error.what());
    }
  }

}

void runIconTests(TestRunner& runner, const std::string& root) {
  iconMatchesSprite(runner, root);
  }

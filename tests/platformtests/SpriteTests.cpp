/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The compiled-in sprites decode, and draw exactly as graphics.c's per-pixel GDI pens drew them */

#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include "Canvas.h"
#include "Compare.h"
#include "GdiSurface.h"
#include "SpriteCache.h"
#include "TestRunner.h"
#include "XpmDecoder.h"

/* selectsprite.c, compiled into this program as C */
extern "C" char** select_sprite(int32_t n);

namespace {

constexpr int32_t SPRITE_COUNT = 368;

const char* const* sprite(int32_t n) {
  return const_cast<const char* const*>(select_sprite(n));
  }

/* graphics.c's _rcs_buildColors and rcs_drawPixmap, kept as they were (one character per pixel) */
void drawPixmapWithGdi(GdiSurface& surface, int32_t dx, int32_t dy, const char* const* data) {
  int                   sizeX = 0, sizeY = 0, numColors = 0, bytesPer = 0;
  int                   none = -1;
  std::vector<COLORREF> colors(256, 0);

  std::sscanf(data[0], "%d %d %d %d", &sizeX, &sizeY, &numColors, &bytesPer);
  for (int x = 1; x <= numColors; x++) {
    char index = 0;
    char temp = 0;
    char buffer[80] = {};
    std::sscanf(data[x], "%c %c %79s", &index, &temp, buffer);
    int colorIndex = static_cast<unsigned char>(index);
    if (buffer[0] != '#') {
      if (std::strcmp(buffer, "None") == 0) none = static_cast<unsigned char>(index);
      }
    else {
      char          channel[8] = {};
      unsigned long red = 0, green = 0, blue = 0;
      std::memcpy(channel, buffer + 1, 4);
      std::sscanf(channel, "%lx", &red);
      std::memcpy(channel, buffer + 5, 4);
      std::sscanf(channel, "%lx", &green);
      std::memcpy(channel, buffer + 9, 4);
      std::sscanf(channel, "%lx", &blue);
      if (red > 255) red /= 256;
      if (blue > 255) blue /= 256;
      if (green > 255) green /= 256;
      colors.at(static_cast<size_t>(colorIndex)) = RGB(red, green, blue);
      }
    }

  for (int y = 0; y < sizeY; y++)
    for (int x = 0; x < sizeX; x++) {
      int colorIndex = static_cast<unsigned char>(data[y + numColors + 1][x]);
      if (colorIndex != none) {
        HPEN    pen = CreatePen(PS_SOLID, 0, colors.at(static_cast<size_t>(colorIndex)));
        HGDIOBJ previous = SelectObject(surface.dc(), pen);
        MoveToEx(surface.dc(), x + dx, y + dy, nullptr);
        LineTo(surface.dc(), x + 1 + dx, y + dy);
        SelectObject(surface.dc(), previous);
        DeleteObject(pen);
        }
      }
  }

void allSpritesDecode(TestRunner& runner) {
  platform::XpmDecoder decoder;
  for (int32_t n = 1; n <= SPRITE_COUNT; n++) {
    std::string detail = "sprite " + std::to_string(n) + ": ";
    try {
      platform::Image image = decoder.decode(sprite(n));
      runner.check(image.width() == 25 && image.height() == 27, "AllSpritesDecode",
                   detail + std::to_string(image.width()) + "x" + std::to_string(image.height()));
      }
    catch (const std::exception& error) {
      runner.check(false, "AllSpritesDecode", detail + error.what());
      }
    }
  }

/* sprite_282 has three colours: a space for "None", "." and "X" */
void spriteTransparency(TestRunner& runner) {
  platform::Image image = platform::XpmDecoder().decode(sprite(282));
  runner.check(!image.opaque(0, 0), "SpriteTransparency", "pixel 0,0 should be transparent");
  runner.check(image.opaque(12, 7) && image.pixel(12, 7) == 0xa5a2a5, "SpriteTransparency",
               "pixel 12,7 should be #a5a2a5");
  }

/* Each sprite through the cache onto a canvas, against graphics.c's pen-per-pixel drawing */
void spritesMatchGdi(TestRunner& runner) {
  platform::SpriteCache cache;
  for (int32_t n = 1; n <= SPRITE_COUNT; n++) {
    platform::Canvas canvas(40, 40);
    GdiSurface       surface(40, 40);
    canvas.fill(0, 0, 40, 40, 0x404040);
    surface.fill(0, 0, 40, 40, 0x404040);
    canvas.image(5, 6, cache.sprite(sprite(n)));
    drawPixmapWithGdi(surface, 5, 6, sprite(n));
    std::string difference = comparePixels(canvas.pixels(), 40, surface, 0, 0, 40, 40);
    runner.check(difference.empty(), "SpritesMatchGdi", "sprite " + std::to_string(n) + " " + difference);
    }
  }

}

void runSpriteTests(TestRunner& runner) {
  allSpritesDecode(runner);
  spriteTransparency(runner);
  spritesMatchGdi(runner);
  }

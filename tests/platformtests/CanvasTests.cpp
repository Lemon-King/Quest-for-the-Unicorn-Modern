/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The canvas primitives against the GDI calls they replace, pixel for pixel */

#include <string>

#include "ApiTypes.h"
#include "Canvas.h"
#include "Compare.h"
#include "GdiSurface.h"
#include "TestRunner.h"
#include "platform.h"

namespace {

constexpr uint32_t WHITE = 0xffffff;
constexpr uint32_t YELLOW = 0xffff00;

std::string describe(int32_t a, int32_t b, int32_t c, int32_t d) {
  return "(" + std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c) + "," + std::to_string(d) + ") ";
  }

/* Every line with both ends in [-12,12] around (32,32); GDI cannot reach past [20,44], so that area is compared */
void canvasLineMatchesGdi(TestRunner& runner) {
  platform::Canvas canvas(64, 64);
  GdiSurface       surface(64, 64);

  for (int32_t x1 = 20; x1 <= 44; x1++)
    for (int32_t y1 = 20; y1 <= 44; y1++)
      for (int32_t x2 = 20; x2 <= 44; x2++)
        for (int32_t y2 = 20; y2 <= 44; y2++) {
          canvas.fill(0, 0, 64, 64, 0);
          surface.clear();
          canvas.line(x1, y1, x2, y2, WHITE);
          surface.line(x1, y1, x2, y2, WHITE);
          std::string difference = comparePixels(canvas.pixels(), 64, surface, 16, 16, 48, 48);
          runner.check(difference.empty(), "CanvasLineMatchesGdi", describe(x1, y1, x2, y2) + difference);
          }
  }

void canvasFillMatchesGdi(TestRunner& runner) {
  struct Box { int32_t x, y, width, height; const char* name; };
  const Box boxes[] = {
    {10, 10, 20, 15, "normal"},
    {30, 10, -20, 15, "reversed in x"},
    {10, 30, 20, -15, "reversed in y"},
    {30, 30, -20, -15, "reversed in both"},
    {10, 10, 0, 15, "zero width"},
    {10, 10, 15, 0, "zero height"},
    {-5, -5, 20, 20, "past the top left"},
    {50, 50, 30, 30, "past the bottom right"},
    {-10, 20, 100, 5, "wider than the surface"},
    };

  for (const Box& box : boxes) {
    platform::Canvas canvas(64, 64);
    GdiSurface       surface(64, 64);
    canvas.fill(box.x, box.y, box.width, box.height, YELLOW);
    surface.fill(box.x, box.y, box.width, box.height, YELLOW);
    std::string difference = comparePixels(canvas.pixels(), 64, surface, 0, 0, 64, 64);
    runner.check(difference.empty(), "CanvasFillMatchesGdi", std::string(box.name) + ": " + difference);
    }
  }

void canvasPointMatchesGdi(TestRunner& runner) {
  struct Spot { int32_t x, y; };
  const Spot spots[] = {
    {10, 10}, {0, 20}, {63, 20}, {20, 0}, {20, 63}, {0, 0}, {63, 63}, {-1, 5}, {64, 5}, {5, -1}, {5, 64},
    };

  for (const Spot& spot : spots) {
    platform::Canvas canvas(64, 64);
    GdiSurface       surface(64, 64);
    canvas.point(spot.x, spot.y, WHITE);
    surface.point(spot.x, spot.y, WHITE);
    std::string difference = comparePixels(canvas.pixels(), 64, surface, 0, 0, 64, 64);
    runner.check(difference.empty(), "CanvasPointMatchesGdi",
                 "(" + std::to_string(spot.x) + "," + std::to_string(spot.y) + ") " + difference);
    }
  }

/* Through the C API, as the game draws text: every byte alone, then a line of text */
void terminalFontMatchesGdi(TestRunner& runner) {
  UINT codePage = GetOEMCP();
  if (codePage != 437) {
    runner.skip("TerminalFontMatchesGdi", "the OEM code page is " + std::to_string(codePage) + ", not 437");
    return;
    }

  for (int code = 1; code < 256; code++) {
    char    glyph[2] = {static_cast<char>(code), 0};
    CANVAS* canvas = canvas_create(32, 32);
    GdiSurface surface(32, 32);
    canvas_text(canvas, terminal_font(), 8, 16, glyph, WHITE);
    surface.text(8, 16, glyph, WHITE);
    std::string difference = comparePixels(canvas->canvas.pixels(), 32, surface, 0, 0, 32, 32);
    runner.check(difference.empty(), "TerminalFontMatchesGdi", "byte " + std::to_string(code) + " " + difference);
    canvas_destroy(canvas);
    }

  const char* line = "Quest for the Unicorn 0123456789";
  CANVAS*     canvas = canvas_create(288, 32);
  GdiSurface  surface(288, 32);
  canvas_text(canvas, terminal_font(), 3, 20, line, YELLOW);
  surface.text(3, 20, line, YELLOW);
  std::string difference = comparePixels(canvas->canvas.pixels(), 288, surface, 0, 0, 288, 32);
  runner.check(difference.empty(), "TerminalFontMatchesGdi", std::string("text line ") + difference);
  canvas_destroy(canvas);
  }

}

void runCanvasTests(TestRunner& runner) {
  canvasLineMatchesGdi(runner);
  canvasFillMatchesGdi(runner);
  canvasPointMatchesGdi(runner);
  terminalFontMatchesGdi(runner);
  }

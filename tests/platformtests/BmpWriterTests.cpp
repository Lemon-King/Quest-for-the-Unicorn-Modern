/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Ctrl+P screenshots keep the layout the GDI build wrote: 24-bit, bottom-up, rows padded to 4 bytes */

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "Canvas.h"
#include "TestRunner.h"
#include "platform.h"

namespace {

uint32_t read16(const std::vector<uint8_t>& bytes, size_t at) {
  return static_cast<uint32_t>(bytes[at]) | (static_cast<uint32_t>(bytes[at + 1]) << 8);
  }

uint32_t read32(const std::vector<uint8_t>& bytes, size_t at) {
  return read16(bytes, at) | (read16(bytes, at + 2) << 16);
  }

std::vector<uint8_t> readFile(const std::string& path) {
  std::vector<uint8_t> bytes;
  std::FILE*           file = std::fopen(path.c_str(), "rb");
  if (file == nullptr) return bytes;
  int c;
  while ((c = std::fgetc(file)) != EOF) bytes.push_back(static_cast<uint8_t>(c));
  std::fclose(file);
  return bytes;
  }

void bmpWriterLayout(TestRunner& runner) {
  char folder[MAX_PATH];
  GetTempPathA(MAX_PATH, folder);
  std::string path = std::string(folder) + "platformtests-layout.bmp";

  /* 3x2: top row red, green, blue; bottom row white, grey, black */
  CANVAS* canvas = canvas_create(3, 2);
  canvas_point(canvas, 0, 0, 0xff0000);
  canvas_point(canvas, 1, 0, 0x00ff00);
  canvas_point(canvas, 2, 0, 0x0000ff);
  canvas_point(canvas, 0, 1, 0xffffff);
  canvas_point(canvas, 1, 1, 0x808080);
  canvas_point(canvas, 2, 1, 0x000000);
  int32_t saved = canvas_save_bmp(canvas, path.c_str());
  canvas_destroy(canvas);
  runner.check(saved == 1, "BmpWriterLayout", "canvas_save_bmp failed");

  std::vector<uint8_t> bytes = readFile(path);
  std::remove(path.c_str());

  const size_t rowSize = 12;   /* 3 pixels x 3 bytes, padded to 4 */
  runner.check(bytes.size() == 54 + rowSize * 2, "BmpWriterLayout", "file is " + std::to_string(bytes.size()) + " bytes");
  if (bytes.size() != 54 + rowSize * 2) return;

  runner.check(bytes[0] == 'B' && bytes[1] == 'M', "BmpWriterLayout", "signature");
  runner.check(read32(bytes, 2) == 54 + rowSize * 2, "BmpWriterLayout", "file size field");
  runner.check(read32(bytes, 6) == 0, "BmpWriterLayout", "reserved fields");
  runner.check(read32(bytes, 10) == 54, "BmpWriterLayout", "pixel offset");
  runner.check(read32(bytes, 14) == 40, "BmpWriterLayout", "info header size");
  runner.check(read32(bytes, 18) == 3 && read32(bytes, 22) == 2, "BmpWriterLayout", "width and height");
  runner.check(read16(bytes, 26) == 1 && read16(bytes, 28) == 24, "BmpWriterLayout", "planes and bit count");
  runner.check(read32(bytes, 30) == 0 && read32(bytes, 34) == rowSize * 2, "BmpWriterLayout", "compression, image size");
  runner.check(read32(bytes, 38) == 2835 && read32(bytes, 42) == 2835, "BmpWriterLayout", "72 dpi");
  runner.check(read32(bytes, 46) == 0 && read32(bytes, 50) == 0, "BmpWriterLayout", "colour counts");

  /* Bottom row first, B,G,R per pixel, then three zero bytes of padding */
  const uint8_t rows[2][12] = {
    {0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0, 0, 0},
    {0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0, 0, 0},
    };
  for (size_t row = 0; row < 2; row++)
    for (size_t i = 0; i < rowSize; i++)
      runner.check(bytes[54 + row * rowSize + i] == rows[row][i], "BmpWriterLayout",
                   "pixel byte " + std::to_string(i) + " of file row " + std::to_string(row));
  }

}

void runBmpWriterTests(TestRunner& runner) {
  bmpWriterLayout(runner);
  }

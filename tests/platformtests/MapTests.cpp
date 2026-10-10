/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The map reader and loader: every character becomes its tile, LF and CRLF files and the DOS end-of-file byte read
   alike, a malformed file is refused with its name and line, and every committed map loads at its index */

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string>
#include <utility>

#include "TestRunner.h"

/* readmap.c and loadmaps.c, compiled into this program as C */
extern "C" int32_t read_map(char* filename, int32_t (*map)[101][101], char* error, int32_t error_size);
extern "C" int32_t load_maps(int32_t (*world)[101][101], char* error, int32_t error_size);

namespace {

using MapTiles = int32_t[101][101];

/* Each map character and the tile it must become */
struct TileCase {
  char    character;
  int32_t tile;
  };

constexpr TileCase TILES[] = {
  {' ', 1},   {'.', 1},   {'#', 2},   {'^', 3},   {'v', 177}, {'~', 4},   {'-', 8},   {'=', 6},   {'(', 9},
  {'!', 17},  {'@', 21},  {'+', 7},   {'\\', 23}, {'/', 22},  {'|', 14},  {'%', 20},  {'s', 13},  {',', 15},
  {'c', 26},  {'t', 27},  {'b', 29},  {'h', 28},  {'}', 32},  {'m', 30},  {'1', 5},   {'A', 11},  {'B', 520},
  {'C', 521}, {'D', 522}, {'E', 523}, {'F', 524}, {'G', 525}, {'H', 526}, {'I', 527}, {'J', 528}, {'K', 529},
  {'L', 530}, {'M', 531}, {'N', 532}, {'O', 533}, {'P', 534}, {'Q', 535}, {'Z', 536}, {'x', 7},   {'R', 7},
  };

/* The files in maps/ */
constexpr const char* COMMITTED[] = {
  "map11", "map12", "map13", "map14", "map21", "map22", "map23", "map24", "map31", "map32", "map33", "map34",
  "radon", "generic", "village", "marduk", "nichor", "eshter",
  };

/* The file each index of maps[] must hold: map11 twice, regions 1-12, the towns, then Eshter's realm */
constexpr const char* LOAD_ORDER[] = {
  "map11", "map11", "map12", "map13", "map14", "map21", "map22", "map23", "map24", "map31", "map32", "map33", "map34",
  "radon", "generic", "village", "marduk", "nichor", "eshter",
  };

std::string tempPath() {
  char folder[MAX_PATH];
  GetTempPathA(MAX_PATH, folder);
  return std::string(folder) + "platformtests-map.map";
  }

/* Line 1 holds every TILES character, then plains; the other lines are forest */
std::string sampleLine(int32_t y) {
  if (y > 1) return std::string(100, '#');

  std::string line;
  for (const TileCase& tile : TILES) line += tile.character;
  return line + std::string(100 - line.size(), '.');
  }

std::string sampleText(int32_t lines, const std::string& ending) {
  std::string text;
  for (int32_t y = 1; y <= lines; y++) text += sampleLine(y) + ending;
  return text;
  }

std::string forest(int32_t lines) {
  std::string text;
  for (int32_t y = 0; y < lines; y++) text += std::string(100, '#') + "\r\n";
  return text;
  }

/* Reads text through a temporary file; the error, or "" when the file was read */
std::string readText(const std::string& text, MapTiles& map) {
  std::string path = tempPath();
  std::FILE*  file = std::fopen(path.c_str(), "wb");
  if (file == nullptr) return "could not create " + path;
  std::fwrite(text.data(), 1, text.size(), file);
  std::fclose(file);

  char    error[256] = "";
  int32_t read = read_map(path.data(), &map, error, static_cast<int32_t>(sizeof(error)));
  std::remove(path.c_str());
  return (read == 1) ? "" : error;
  }

/* load_maps() reads the maps folder of the working directory, so it runs in the repo root; the error, or "" */
std::string loadInRoot(const std::string& root, MapTiles* world) {
  char previous[MAX_PATH];
  char error[256] = "";
  GetCurrentDirectoryA(MAX_PATH, previous);
  SetCurrentDirectoryA(root.c_str());
  int32_t loaded = load_maps(world, error, static_cast<int32_t>(sizeof(error)));
  SetCurrentDirectoryA(previous);
  return (loaded == 1) ? "" : error;
  }

void mapTiles(TestRunner& runner) {
  static MapTiles map;
  std::string     error = readText(sampleText(100, "\r\n"), map);
  runner.check(error.empty(), "MapTiles", error);
  if (!error.empty()) return;

  for (size_t i = 0; i < std::size(TILES); i++)
    runner.check(map[1][i + 1] == TILES[i].tile, "MapTiles",
                 std::string("'") + TILES[i].character + "' read as " + std::to_string(map[1][i + 1]) + ", expected " +
                   std::to_string(TILES[i].tile));

  for (int32_t i = 0; i <= 100; i++)
    runner.check(map[0][i] == 1 && map[i][0] == 1, "MapTiles", "row 0 and column 0 must be plains, at " + std::to_string(i));
  runner.check(map[100][100] == 2, "MapTiles", "line 100, column 100 must be forest");
  }

/* LF, a last line without its ending, the DOS end-of-file byte and trailing empty lines read as the CRLF file does */
void mapLineEndings(TestRunner& runner) {
  static MapTiles expected;
  static MapTiles actual;
  readText(sampleText(100, "\r\n"), expected);

  const std::pair<const char*, std::string> variants[] = {
    {"LF", sampleText(100, "\n")},
    {"no ending on the last line", sampleText(99, "\r\n") + sampleLine(100)},
    {"Ctrl+Z after the last line", sampleText(100, "\r\n") + "\x1a"},
    {"Ctrl+Z ending the last line", sampleText(99, "\r\n") + sampleLine(100) + "\x1a"},
    {"empty lines after the last line", sampleText(100, "\r\n") + "\r\n\n\r\n"},
    };
  for (const auto& [name, text] : variants) {
    std::string error = readText(text, actual);
    runner.check(error.empty(), "MapLineEndings", std::string(name) + ": " + error);
    runner.check(error.empty() && std::memcmp(expected, actual, sizeof(MapTiles)) == 0, "MapLineEndings",
                 std::string(name) + ": the tiles differ from the CRLF file's");
    }
  }

/* A malformed or missing file is refused, and the message names the file and the line */
void mapErrors(TestRunner& runner) {
  static MapTiles map;
  std::string     path = tempPath();

  const std::pair<std::string, std::string> cases[] = {
    {forest(99), path + " has 99 lines, expected 100"},
    {"", path + " has 0 lines, expected 100"},
    {forest(11) + std::string(99, '#') + "\r\n" + forest(88), path + ", line 12 is not 100 characters long"},
    {forest(11) + std::string(101, '#') + "\r\n" + forest(88), path + ", line 12 is not 100 characters long"},
    {forest(101), path + " has more than 100 lines"},
    };
  for (const auto& [text, expected] : cases) {
    std::string error = readText(text, map);
    runner.check(error == expected, "MapErrors", "\"" + error + "\", expected \"" + expected + "\"");
    }

  char    error[256] = "";
  int32_t read = read_map(path.data(), &map, error, static_cast<int32_t>(sizeof(error)));
  runner.check(read == 0 && error == "could not open " + path, "MapErrors", std::string("missing file: \"") + error + "\"");
  }

void committedMaps(TestRunner& runner, const std::string& root) {
  if (root.empty()) {
    runner.check(false, "CommittedMaps", "needs the repo root as its first argument, to find maps\\");
    return;
    }

  static MapTiles map;
  for (const char* name : COMMITTED) {
    std::string file = root + "\\maps\\" + name + ".map";
    char        error[256] = "";
    int32_t     read = read_map(file.data(), &map, error, static_cast<int32_t>(sizeof(error)));
    runner.check(read == 1, "CommittedMaps", error);
    }
  }

void loadedMapsInOrder(TestRunner& runner, const std::string& root) {
  if (root.empty()) return;

  static MapTiles world[std::size(LOAD_ORDER)];
  std::string     error = loadInRoot(root, world);
  runner.check(error.empty(), "LoadedMapsInOrder", error);
  if (!error.empty()) return;

  static MapTiles map;
  for (size_t i = 0; i < std::size(LOAD_ORDER); i++) {
    std::string file = root + "\\maps\\" + LOAD_ORDER[i] + ".map";
    char        text[256] = "";
    int32_t     read = read_map(file.data(), &map, text, static_cast<int32_t>(sizeof(text)));
    runner.check(read == 1 && std::memcmp(world[i], map, sizeof(MapTiles)) == 0, "LoadedMapsInOrder",
                 "maps[" + std::to_string(i) + "] is not " + LOAD_ORDER[i] + ".map");
    }
  }

}

void runMapTests(TestRunner& runner, const std::string& root) {
  mapTiles(runner);
  mapLineEndings(runner);
  mapErrors(runner);
  committedMaps(runner, root);
  loadedMapsInOrder(runner, root);
  }

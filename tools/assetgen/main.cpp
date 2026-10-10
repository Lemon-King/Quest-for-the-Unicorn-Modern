/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Generates committed assets from Windows: "assetgen font <TerminalFont.h>" captures the Terminal font's glyphs */

#include <cstdio>
#include <exception>
#include <string>

#include "FontCapture.h"

int main(int argc, char** argv) {
  try {
    std::string command = (argc > 1) ? argv[1] : "";
    if (command == "font" && argc == 3) {
      FontCapture capture;
      capture.writeHeader(argv[2]);
      return 0;
      }

    std::fputs("usage: assetgen font <TerminalFont.h>\n", stderr);
    return 2;
    }
  catch (const std::exception& error) {
    std::fprintf(stderr, "assetgen: %s\n", error.what());
    return 1;
    }
  }

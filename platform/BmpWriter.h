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

namespace platform {

class Canvas;

/* A canvas as a 24-bit uncompressed BMP: bottom-up rows padded to 4 bytes, 72 dpi, as the GDI build's Ctrl+P wrote */
class BmpWriter {
  public:
    explicit BmpWriter(const Canvas& canvas);
    bool write(const char* filename) const;

  private:
    const Canvas& canvas_;
  };

}

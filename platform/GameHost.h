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

#include <cstdint>

#include "Canvas.h"

namespace platform {

/* How the game meets the window: the game draws and asks for keys; the window shows frames and collects keys */
class GameHost {
  public:
    virtual ~GameHost() = default;

    virtual int run() = 0;
    virtual void present(Canvas& canvas) = 0;
    virtual int32_t peekKey() = 0;
    virtual int32_t readKey() = 0;
    virtual void sleep(int32_t milliseconds) = 0;
    virtual bool closed() const = 0;
  };

}

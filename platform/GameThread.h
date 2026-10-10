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

#include "GameHost.h"
#include "GameWindow.h"

namespace platform {

/* Runs the game on its own thread with room for the game's large stack frames, and tells the window when it ends */
class GameThread {
  public:
    GameThread(GameHost& host, GameWindow& window);
    ~GameThread();

    GameThread(const GameThread&) = delete;
    GameThread& operator=(const GameThread&) = delete;

  private:
    static unsigned __stdcall entry(void* self);

    GameHost&   host_;
    GameWindow& window_;
    HANDLE      thread_;
  };

}

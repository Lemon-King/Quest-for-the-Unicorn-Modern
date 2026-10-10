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

#include "FrameMailbox.h"
#include "GameHost.h"
#include "GameWindow.h"
#include "InputQueue.h"

namespace platform {

/* The game on its own thread, the window on this one: the game never waits for the window, and the window never
   waits for the game */
class ThreadedHost final : public GameHost {
  public:
    ThreadedHost(FrameMailbox& frames, InputQueue& input, GameWindow& window);

    int run() override;
    void present(Canvas& canvas) override;
    int32_t peekKey() override;
    int32_t readKey() override;
    void sleep(int32_t milliseconds) override;
    bool closed() const override;

  private:
    FrameMailbox& frames_;
    InputQueue&   input_;
    GameWindow&   window_;
  };

}

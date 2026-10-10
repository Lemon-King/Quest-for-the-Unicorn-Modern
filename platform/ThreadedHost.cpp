/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "ThreadedHost.h"

#include <windows.h>

#include <algorithm>

#include "GameThread.h"

namespace platform {

ThreadedHost::ThreadedHost(FrameMailbox& frames, InputQueue& input, GameWindow& window)
  : frames_(frames), input_(input), window_(window) {
  }

/* The window's message loop, until the game thread ends */
int ThreadedHost::run() {
  GameThread game(*this, window_);
  MSG        message;

  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
    }
  return static_cast<int>(message.wParam);
  }

/* The rest run on the game thread */
void ThreadedHost::present(Canvas& canvas) {
  if (frames_.publish(canvas)) window_.requestFrame();
  }

int32_t ThreadedHost::peekKey() {
  return input_.peek();
  }

int32_t ThreadedHost::readKey() {
  return input_.waitPop();
  }

/* The window thread keeps the window answering meanwhile */
void ThreadedHost::sleep(int32_t milliseconds) {
  Sleep(static_cast<DWORD>(std::max(milliseconds, 0)));
  }

bool ThreadedHost::closed() const {
  return input_.closed();
  }

}

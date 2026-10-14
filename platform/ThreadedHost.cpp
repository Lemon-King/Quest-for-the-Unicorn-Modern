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

/* The window's message loop, until the game thread ends; the window presents its frame clock's ticks between messages */
int ThreadedHost::run() {
  GameThread game(*this, window_);
  MSG        message;

  for (;;) {
    window_.awaitMessage();

    /* Special keys go to the window before TranslateMessage could turn them into characters */
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) return static_cast<int>(message.wParam);
      if (window_.translate(message)) continue;
      TranslateMessage(&message);
      DispatchMessageW(&message);
      }
    }
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

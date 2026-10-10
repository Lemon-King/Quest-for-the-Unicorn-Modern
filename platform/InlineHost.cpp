/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "InlineHost.h"

#include <windows.h>

#include <algorithm>

#include "ApiTypes.h"
#include "platform.h"

namespace platform {

InlineHost::InlineHost(FrameMailbox& frames, InputQueue& input, GameWindow& window)
  : frames_(frames), input_(input), window_(window) {
  }

/* The game runs on this thread until the player saves and leaves */
int InlineHost::run() {
  GAME_HOST handle{*this};
  run_game(&handle);
  return 0;
  }

/* Shown at once: there is no other thread to show it */
void InlineHost::present(Canvas& canvas) {
  if (frames_.publish(canvas)) window_.showFrame();
  }

int32_t InlineHost::peekKey() {
  pump(false);
  return input_.peek();
  }

int32_t InlineHost::readKey() {
  while (input_.peek() == HOST_KEY_NONE) pump(true);
  return input_.pop();
  }

/* The window is not serviced while the game sleeps, as before */
void InlineHost::sleep(int32_t milliseconds) {
  Sleep(static_cast<DWORD>(std::max(milliseconds, 0)));
  }

bool InlineHost::closed() const {
  return input_.closed();
  }

/* Handles every waiting window message; with wait, blocks until at least one arrives */
void InlineHost::pump(bool wait) {
  MSG message;
  if (wait && GetMessageW(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
    }
  while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
    }
  }

}

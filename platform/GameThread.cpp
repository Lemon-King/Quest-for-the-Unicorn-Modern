/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "GameThread.h"

#include <process.h>

#include <stdexcept>

#include "ApiTypes.h"
#include "platform.h"

namespace platform {

namespace {

/* load.c returns a DUNGEON_LEVEL (about 272 KB) by value, and Debug frames are larger still; the main thread's
   reserve is the same 8 MB */
const unsigned STACK_RESERVE = 8 * 1024 * 1024;

}

GameThread::GameThread(GameHost& host, GameWindow& window) : host_(host), window_(window), thread_(nullptr) {
  thread_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, STACK_RESERVE, entry, this,
                                                    STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr));
  if (thread_ == nullptr) throw std::runtime_error("could not start the game thread");
  }

/* The game has ended or is about to: its thread posted the end before returning */
GameThread::~GameThread() {
  WaitForSingleObject(thread_, INFINITE);
  CloseHandle(thread_);
  }

unsigned __stdcall GameThread::entry(void* self) {
  GameThread* thread = static_cast<GameThread*>(self);
  GAME_HOST   handle{thread->host_};

  run_game(&handle);
  thread->window_.gameEnded();
  return 0;
  }

}

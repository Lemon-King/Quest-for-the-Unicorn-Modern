/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The host part of platform.h. There is no way back into C from a failure here, so one ends the game with a
   message */

#include "platform.h"

#include <windows.h>

#include <exception>

#include "ApiTypes.h"
#include "ErrorReport.h"

namespace {

[[noreturn]] void fail(const std::exception& error) {
  platform::ErrorReport(error).show();
  ExitProcess(1);
  }

}

extern "C" void host_present(GAME_HOST* host, CANVAS* canvas) {
  try {
    host->host.present(canvas->canvas);
    }
  catch (const std::exception& error) {
    fail(error);
    }
  }

extern "C" int32_t host_peek_key(GAME_HOST* host) {
  try {
    return host->host.peekKey();
    }
  catch (const std::exception& error) {
    fail(error);
    }
  }

extern "C" int32_t host_read_key(GAME_HOST* host) {
  try {
    return host->host.readKey();
    }
  catch (const std::exception& error) {
    fail(error);
    }
  }

extern "C" void host_sleep(GAME_HOST* host, int32_t milliseconds) {
  try {
    host->host.sleep(milliseconds);
    }
  catch (const std::exception& error) {
    fail(error);
    }
  }

extern "C" int32_t host_closed(GAME_HOST* host) {
  try {
    return host->host.closed() ? 1 : 0;
    }
  catch (const std::exception& error) {
    fail(error);
    }
  }

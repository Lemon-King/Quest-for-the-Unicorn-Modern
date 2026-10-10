/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "Direct3DError.h"

#include <cstdio>
#include <string>

namespace platform {

namespace {

std::string describe(HRESULT result, const char* call) {
  char text[160];
  std::snprintf(text, sizeof(text), "%s failed (0x%08X)", call, static_cast<unsigned>(result));
  return text;
  }

}

Direct3DError::Direct3DError(HRESULT result, const char* call)
  : std::runtime_error(describe(result, call)), result_(result) {
  }

HRESULT Direct3DError::result() const {
  return result_;
  }

/* The GPU was reset or removed: everything made from the device has to be made again */
bool Direct3DError::deviceLost() const {
  return result_ == DXGI_ERROR_DEVICE_REMOVED || result_ == DXGI_ERROR_DEVICE_RESET;
  }

void Direct3DError::check(HRESULT result, const char* call) {
  if (FAILED(result)) throw Direct3DError(result, call);
  }

}

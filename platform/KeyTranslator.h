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

#include <cstdint>
#include <optional>

#include "KeyRoute.h"

namespace platform {

/* Window key messages to the game's key codes. Characters come from WM_CHAR, so the keyboard layout, Shift and Caps
   Lock are Windows' business; key-downs only cover keys that carry no character. */
class KeyTranslator {
  public:
    KeyRoute keyDown(UINT message, WPARAM virtualKey, LPARAM flags);
    void keyUp(WPARAM virtualKey);
    std::optional<int32_t> character(WPARAM code) const;
    void reset();

  private:
    bool shift_ = false;
    bool control_ = false;
  };

}

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

namespace platform {

/* Window key messages to the game's key codes, as the GDI build mapped them: Shift and Caps Lock for letters, US
   punctuation, Ctrl+letter as control codes, arrows and keypad digits as directions, F1-F10 */
class KeyTranslator {
  public:
    std::optional<int32_t> keyDown(WPARAM virtualKey, bool capsLock);
    void keyUp(WPARAM virtualKey);
    void reset();

  private:
    bool shift_ = false;
    bool control_ = false;
  };

}

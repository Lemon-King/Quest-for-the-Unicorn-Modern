/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "KeyTranslator.h"

#include "../terminal.h"

namespace platform {

namespace {

/* Scan codes 0x02-0x0D: the number row 1..9, 0, -, = carries the Alt layer the Alt+F-keys used to */
constexpr int32_t ALT_ROW[12] = { KY_A_1, KY_A_2, KY_A_3, KY_A_4, KY_A_5, KY_A_6, KY_A_7, KY_A_8, KY_A_9, KY_A_0,
                                  KY_A_MINUS, KY_A_EQUAL };

constexpr int32_t PLAIN_F[12] = { KY_F1, KY_F2, KY_F3, KY_F4, KY_F5, KY_F6, KY_F7, KY_F8, KY_F9, KY_F10, KY_F11,
                                  KY_F12 };
constexpr int32_t SHIFT_F[12] = { KY_S_F1, KY_S_F2, KY_S_F3, KY_S_F4, KY_S_F5, KY_S_F6, KY_S_F7, KY_S_F8, KY_S_F9,
                                  KY_S_F10, KY_S_F11, KY_S_F12 };
constexpr int32_t CONTROL_F[12] = { KY_C_F1, KY_C_F2, KY_C_F3, KY_C_F4, KY_C_F5, KY_C_F6, KY_C_F7, KY_C_F8, KY_C_F9,
                                    KY_C_F10, KY_C_F11, KY_C_F12 };

/* lParam bits of a key message */
constexpr LPARAM ALT_DOWN = LPARAM{1} << 29;

}

/* F10 arrives as WM_SYSKEYDOWN without Alt, and is an F-key like the others */
KeyRoute KeyTranslator::keyDown(UINT message, WPARAM virtualKey, LPARAM flags) {
  bool    alt = message == WM_SYSKEYDOWN && (flags & ALT_DOWN) != 0;
  int32_t scan = static_cast<int32_t>((flags >> 16) & 0xff);

  switch (virtualKey) {
    case VK_SHIFT:
      shift_ = true;
      return KeyRoute::swallow();
    case VK_CONTROL:
      control_ = true;
      return KeyRoute::swallow();
    case VK_MENU:
      return KeyRoute::swallow();
    }

  if (alt) {
    if (virtualKey == VK_F4) return KeyRoute::windows();
    if (virtualKey == VK_RETURN) return KeyRoute::fullscreen();
    if (scan >= 0x02 && scan <= 0x0d) return KeyRoute::game(ALT_ROW[scan - 0x02]);
    return KeyRoute::windows();
    }

  /* Ctrl wins over Shift */
  if (virtualKey >= VK_F1 && virtualKey <= VK_F12) {
    size_t index = virtualKey - VK_F1;
    if (control_) return KeyRoute::game(CONTROL_F[index]);
    if (shift_) return KeyRoute::game(SHIFT_F[index]);
    return KeyRoute::game(PLAIN_F[index]);
    }

  /* As a control code Ctrl+M would be 13, the same as Enter */
  if (virtualKey == 'M' && control_) return KeyRoute::game(KY_MUSIC);

  /* The keypad moves with or without NumLock */
  switch (virtualKey) {
    case VK_UP:    return KeyRoute::game('8');
    case VK_DOWN:  return KeyRoute::game('2');
    case VK_LEFT:  return KeyRoute::game('4');
    case VK_RIGHT: return KeyRoute::game('6');
    case VK_HOME:  return KeyRoute::game('7');
    case VK_PRIOR: return KeyRoute::game('9');
    case VK_END:   return KeyRoute::game('1');
    case VK_NEXT:  return KeyRoute::game('3');
    case VK_CLEAR: return KeyRoute::game('5');
    }

  if (virtualKey >= VK_NUMPAD0 && virtualKey <= VK_NUMPAD9)
    return KeyRoute::game('0' + static_cast<int32_t>(virtualKey - VK_NUMPAD0));

  /* Everything else is a character key, or no key the game knows: TranslateMessage decides */
  return KeyRoute::windows();
  }

/* Shift and Ctrl come from the window's own key messages, not GetKeyState(), so posted keys carry them too */
void KeyTranslator::keyUp(WPARAM virtualKey) {
  if (virtualKey == VK_SHIFT) shift_ = false;
  if (virtualKey == VK_CONTROL) control_ = false;
  }

/* The game's text, names and save files are ASCII; anything beyond is dropped */
std::optional<int32_t> KeyTranslator::character(WPARAM code) const {
  if (code == 0 || code > 127) return std::nullopt;
  return static_cast<int32_t>(code);
  }

/* The window lost the keyboard, so no key-up will come for keys held now */
void KeyTranslator::reset() {
  shift_ = false;
  control_ = false;
  }

}

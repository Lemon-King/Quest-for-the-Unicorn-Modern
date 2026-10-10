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

/* Shift with '0'..'9' */
constexpr char SHIFTED_DIGITS[] = ")!@#$%^&*(";

/* VK_OEM_1 (186) .. VK_OEM_3 (192) and VK_OEM_4 (219) .. VK_OEM_7 (222) on a US keyboard, without and with Shift */
constexpr char PUNCTUATION[] = ";=,-./`";
constexpr char SHIFTED_PUNCTUATION[] = ":+<_>?~";
constexpr char BRACKETS[] = "[\\]'";
constexpr char SHIFTED_BRACKETS[] = "{|}\"";

constexpr int32_t F_KEYS[] = {KY_F1, KY_F2, KY_F3, KY_F4, KY_F5, KY_F6, KY_F7, KY_F8, KY_F9, KY_F10};

}

/* Nothing for modifiers and for keys the GDI build only turned into stray codes */
std::optional<int32_t> KeyTranslator::keyDown(WPARAM virtualKey, bool capsLock) {
  int32_t key = static_cast<int32_t>(virtualKey);

  if (key == VK_SHIFT) {
    shift_ = true;
    return std::nullopt;
    }
  if (key == VK_CONTROL) {
    control_ = true;
    return std::nullopt;
    }

  /* Ctrl+M would be 13, the same as Enter */
  if (key == 'M' && control_) return KY_MUSIC;
  if (key >= 'A' && key <= 'Z') {
    if (control_) return key - 64;
    return (shift_ != capsLock) ? key : key + 32;
    }
  if (key >= '0' && key <= '9') return shift_ ? static_cast<int32_t>(SHIFTED_DIGITS[key - '0']) : key;

  switch (key) {
    case 8:
    case 9:
    case 13:
    case 27:
    case 32:
      return key;
    case VK_UP:
      return '8';
    case VK_DOWN:
      return '2';
    case VK_LEFT:
      return '4';
    case VK_RIGHT:
      return '6';
    }

  if (key >= VK_OEM_1 && key <= VK_OEM_3)
    return static_cast<int32_t>((shift_ ? SHIFTED_PUNCTUATION : PUNCTUATION)[key - VK_OEM_1]);
  if (key >= VK_OEM_4 && key <= VK_OEM_7)
    return static_cast<int32_t>((shift_ ? SHIFTED_BRACKETS : BRACKETS)[key - VK_OEM_4]);
  if (key >= VK_NUMPAD1 && key <= VK_NUMPAD9) return '1' + (key - VK_NUMPAD1);
  if (key >= VK_F1 && key <= VK_F10) return F_KEYS[key - VK_F1];
  return std::nullopt;
  }

void KeyTranslator::keyUp(WPARAM virtualKey) {
  if (virtualKey == VK_SHIFT) shift_ = false;
  if (virtualKey == VK_CONTROL) control_ = false;
  }

/* The window lost the keyboard, so no key-up will come for keys held now */
void KeyTranslator::reset() {
  shift_ = false;
  control_ = false;
  }

}

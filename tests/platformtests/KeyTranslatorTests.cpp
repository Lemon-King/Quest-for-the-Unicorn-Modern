/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Key messages give the codes graphics.c's _rcs_winTranslate and terminal.c's keytrans() gave */

#include <optional>
#include <string>
#include <vector>

#include "KeyTranslator.h"
#include "TestRunner.h"

namespace {

constexpr int32_t NOTHING = -1000;

/* Presses the modifiers, then the key, and releases the modifiers again */
int32_t press(platform::KeyTranslator& keys, const std::vector<WPARAM>& modifiers, WPARAM key, bool capsLock) {
  for (WPARAM modifier : modifiers) keys.keyDown(modifier, capsLock);
  std::optional<int32_t> code = keys.keyDown(key, capsLock);
  for (WPARAM modifier : modifiers) keys.keyUp(modifier);
  return code ? *code : NOTHING;
  }

void keyTranslatorMapsLikeGdiBuild(TestRunner& runner) {
  struct Case { std::vector<WPARAM> modifiers; WPARAM key; bool capsLock; int32_t expected; const char* name; };
  const Case cases[] = {
    {{}, 'A', false, 'a', "A"},
    {{VK_SHIFT}, 'A', false, 'A', "Shift+A"},
    {{}, 'A', true, 'A', "Caps Lock, A"},
    {{VK_SHIFT}, 'A', true, 'a', "Caps Lock, Shift+A"},
    {{VK_CONTROL}, 'P', false, 16, "Ctrl+P"},
    {{VK_CONTROL}, 'M', false, 238, "Ctrl+M"},
    {{}, '1', false, '1', "1"},
    {{VK_SHIFT}, '1', false, '!', "Shift+1"},
    {{}, VK_OEM_1, false, ';', "OEM 186"},
    {{VK_SHIFT}, VK_OEM_1, false, ':', "Shift+OEM 186"},
    {{}, VK_OEM_7, false, '\'', "OEM 222"},
    {{VK_SHIFT}, VK_OEM_7, false, '"', "Shift+OEM 222"},
    {{}, VK_RETURN, false, 13, "Enter"},
    {{}, VK_ESCAPE, false, 27, "Esc"},
    {{}, VK_UP, false, '8', "Up"},
    {{}, VK_NUMPAD7, false, '7', "keypad 7"},
    {{}, VK_F1, false, 161, "F1"},
    {{}, VK_F10, false, 160, "F10"},
    {{VK_SHIFT}, VK_F2, false, 162, "Shift+F2"},
    {{}, VK_F11, false, NOTHING, "F11"},
    {{}, VK_CAPITAL, false, NOTHING, "Caps Lock"},
    {{}, VK_SHIFT, false, NOTHING, "Shift"},
    {{}, VK_HOME, false, NOTHING, "Home"},
    };

  platform::KeyTranslator keys;
  for (const Case& item : cases) {
    int32_t code = press(keys, item.modifiers, item.key, item.capsLock);
    runner.check(code == item.expected, "KeyTranslatorMapsLikeGdiBuild",
                 std::string(item.name) + " gave " + std::to_string(code) + ", expected " + std::to_string(item.expected));
    }

  /* Shift stays down until its key-up, and a lost focus forgets it */
  keys.keyDown(VK_SHIFT, false);
  runner.check(keys.keyDown('B', false) == 'B', "KeyTranslatorMapsLikeGdiBuild", "B while Shift is held should be B");
  keys.keyUp(VK_SHIFT);
  runner.check(keys.keyDown('A', false) == 'a', "KeyTranslatorMapsLikeGdiBuild", "A after Shift's key-up should be a");
  keys.keyDown(VK_SHIFT, false);
  keys.reset();
  runner.check(keys.keyDown('A', false) == 'a', "KeyTranslatorMapsLikeGdiBuild", "A after reset() should be a");
  }

}

void runKeyTranslatorTests(TestRunner& runner) {
  keyTranslatorMapsLikeGdiBuild(runner);
  }

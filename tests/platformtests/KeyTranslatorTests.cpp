/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Key-down messages are routed to a game key, nowhere, Windows or the fullscreen toggle; characters come from WM_CHAR */

#include <optional>
#include <string>
#include <vector>

#include "KeyTranslator.h"
#include "TestRunner.h"

using platform::KeyRoute;

namespace {

/* lParam with a scan code, and the Alt context bit when alt */
LPARAM flags(int32_t scan, bool alt) {
  return (static_cast<LPARAM>(scan) << 16) | (alt ? (LPARAM{1} << 29) : 0);
  }

const char* describe(KeyRoute::Target target) {
  switch (target) {
    case KeyRoute::Target::Game:       return "Game";
    case KeyRoute::Target::Swallow:    return "Swallow";
    case KeyRoute::Target::Windows:    return "Windows";
    case KeyRoute::Target::Fullscreen: return "Fullscreen";
    }
  return "?";
  }

/* One key-down after some modifiers pressed and held */
struct Case {
  std::vector<WPARAM> held;
  UINT                message;
  WPARAM              key;
  LPARAM              lParam;
  KeyRoute::Target    target;
  int32_t             code;
  const char*         name;
  };

void keyTranslatorRoutes(TestRunner& runner) {
  using Target = KeyRoute::Target;
  const Case cases[] = {
    {{}, WM_KEYDOWN, VK_F11, 0, Target::Game, 230, "F11"},
    {{VK_SHIFT}, WM_KEYDOWN, VK_F11, 0, Target::Game, 232, "Shift+F11"},
    {{VK_CONTROL}, WM_KEYDOWN, VK_F12, 0, Target::Game, 237, "Ctrl+F12"},
    {{VK_CONTROL, VK_SHIFT}, WM_KEYDOWN, VK_F1, 0, Target::Game, 221, "Ctrl+Shift+F1 (Ctrl wins)"},
    {{}, WM_SYSKEYDOWN, VK_F10, flags(0x44, false), Target::Game, 160, "F10 as WM_SYSKEYDOWN"},
    {{}, WM_SYSKEYDOWN, '1', flags(0x02, true), Target::Game, 211, "Alt+1"},
    {{}, WM_SYSKEYDOWN, '0', flags(0x0b, true), Target::Game, 210, "Alt+0"},
    {{}, WM_SYSKEYDOWN, VK_OEM_MINUS, flags(0x0c, true), Target::Game, 234, "Alt+-"},
    {{}, WM_SYSKEYDOWN, VK_OEM_PLUS, flags(0x0d, true), Target::Game, 235, "Alt+="},
    {{}, WM_SYSKEYDOWN, VK_F4, flags(0x3e, true), Target::Windows, 0, "Alt+F4"},
    {{}, WM_SYSKEYDOWN, VK_RETURN, flags(0x1c, true), Target::Fullscreen, 0, "Alt+Enter"},
    {{}, WM_SYSKEYDOWN, VK_SPACE, flags(0x39, true), Target::Windows, 0, "Alt+Space"},
    {{}, WM_SYSKEYDOWN, 'A', flags(0x1e, true), Target::Windows, 0, "Alt+A"},
    {{}, WM_KEYDOWN, VK_HOME, 0, Target::Game, '7', "Home"},
    {{}, WM_KEYDOWN, VK_NEXT, 0, Target::Game, '3', "PgDn"},
    {{}, WM_KEYDOWN, VK_CLEAR, 0, Target::Game, '5', "keypad 5 without NumLock"},
    {{}, WM_KEYDOWN, VK_NUMPAD0, 0, Target::Game, '0', "keypad 0"},
    {{}, WM_KEYDOWN, VK_UP, 0, Target::Game, '8', "Up"},
    {{VK_CONTROL}, WM_KEYDOWN, 'M', 0, Target::Game, 238, "Ctrl+M"},
    {{VK_CONTROL}, WM_KEYDOWN, 'P', 0, Target::Windows, 0, "Ctrl+P (a WM_CHAR of 16)"},
    {{}, WM_KEYDOWN, 'A', 0, Target::Windows, 0, "A"},
    {{}, WM_KEYDOWN, VK_CAPITAL, 0, Target::Windows, 0, "Caps Lock"},
    {{}, WM_KEYDOWN, VK_DELETE, 0, Target::Windows, 0, "Delete"},
    {{}, WM_KEYDOWN, VK_SHIFT, 0, Target::Swallow, 0, "Shift"},
    {{}, WM_SYSKEYDOWN, VK_MENU, flags(0x38, true), Target::Swallow, 0, "Alt"},
    };

  for (const Case& item : cases) {
    platform::KeyTranslator keys;
    for (WPARAM modifier : item.held) keys.keyDown(WM_KEYDOWN, modifier, 0);
    KeyRoute route = keys.keyDown(item.message, item.key, item.lParam);
    bool     same = route.target() == item.target && (item.target != Target::Game || route.key() == item.code);
    runner.check(same, "KeyTranslatorRoutes",
                 std::string(item.name) + " gave " + describe(route.target()) + " " + std::to_string(route.key()) +
                 ", expected " + describe(item.target) + " " + std::to_string(item.code));
    }

  /* A released Shift no longer modifies, and neither does one held when the window lost the keyboard */
  platform::KeyTranslator keys;
  keys.keyDown(WM_KEYDOWN, VK_SHIFT, 0);
  keys.keyUp(VK_SHIFT);
  runner.check(keys.keyDown(WM_KEYDOWN, VK_F1, 0).key() == 161, "KeyTranslatorRoutes", "F1 after Shift's key-up");
  keys.keyDown(WM_KEYDOWN, VK_CONTROL, 0);
  keys.reset();
  runner.check(keys.keyDown(WM_KEYDOWN, VK_F1, 0).key() == 161, "KeyTranslatorRoutes", "F1 after reset()");

  struct Character { WPARAM code; std::optional<int32_t> expected; };
  const Character characters[] = { {'A', 65}, {16, 16}, {127, 127}, {0xE9, std::nullopt}, {0, std::nullopt} };
  for (const Character& item : characters) {
    std::optional<int32_t> code = keys.character(item.code);
    runner.check(code == item.expected, "KeyTranslatorRoutes",
                 "character(" + std::to_string(item.code) + ") gave " + (code ? std::to_string(*code) : "none"));
    }
  }

}

void runKeyTranslatorTests(TestRunner& runner) {
  keyTranslatorRoutes(runner);
  }

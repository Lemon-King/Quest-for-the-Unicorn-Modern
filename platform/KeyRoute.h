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

#include <cstdint>

namespace platform {

/* Where a key-down message goes: a game key, nowhere, Windows (characters, Alt+F4), or the fullscreen toggle */
class KeyRoute {
  public:
    enum class Target { Game, Swallow, Windows, Fullscreen };

    static KeyRoute game(int32_t key);
    static KeyRoute swallow();
    static KeyRoute windows();
    static KeyRoute fullscreen();

    Target target() const;
    int32_t key() const;

  private:
    KeyRoute(Target target, int32_t key);

    Target  target_;
    int32_t key_;
  };

}

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "KeyRoute.h"

namespace platform {

KeyRoute::KeyRoute(Target target, int32_t key) : target_(target), key_(key) {
  }

KeyRoute KeyRoute::game(int32_t key) {
  return KeyRoute(Target::Game, key);
  }

KeyRoute KeyRoute::swallow() {
  return KeyRoute(Target::Swallow, 0);
  }

KeyRoute KeyRoute::windows() {
  return KeyRoute(Target::Windows, 0);
  }

KeyRoute KeyRoute::fullscreen() {
  return KeyRoute(Target::Fullscreen, 0);
  }

KeyRoute::Target KeyRoute::target() const {
  return target_;
  }

/* Meaningful only for Target::Game; the others carry 0 */
int32_t KeyRoute::key() const {
  return key_;
  }

}

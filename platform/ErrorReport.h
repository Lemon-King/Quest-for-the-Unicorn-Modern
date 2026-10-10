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

#include <exception>
#include <string>

namespace platform {

/* A fatal error as the player sees it: a message box saying the game has to close, and why */
class ErrorReport {
  public:
    explicit ErrorReport(const std::exception& error);
    void show() const;

  private:
    std::wstring text_;
  };

}

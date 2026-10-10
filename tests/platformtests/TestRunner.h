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

#include <string>

/* Counts checks and prints the failures; the exit code is the number of failed checks, capped at 255 */
class TestRunner {
  public:
    void check(bool passed, const std::string& test, const std::string& detail);
    void skip(const std::string& test, const std::string& reason);
    int finish() const;

  private:
    int passed_ = 0;
    int failed_ = 0;
  };

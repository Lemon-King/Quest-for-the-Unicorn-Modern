/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The error part of platform.h. No exception crosses into C; the caller ends the game either way */

#include "platform.h"

#include <exception>
#include <stdexcept>

#include "ErrorReport.h"

extern "C" void show_fatal_error(const char* message) {
  try {
    platform::ErrorReport(std::runtime_error(message)).show();
    }
  catch (const std::exception&) {
    }
  }

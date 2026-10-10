/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "ErrorReport.h"

#include <windows.h>

namespace platform {

namespace {

/* Exception messages are narrow; the manifest makes the narrow code page UTF-8 */
std::wstring widen(const char* text) {
  int length = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
  if (length <= 1) return std::wstring();
  std::wstring wide(static_cast<size_t>(length), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, text, -1, wide.data(), length);
  wide.resize(static_cast<size_t>(length - 1));
  return wide;
  }

}

ErrorReport::ErrorReport(const std::exception& error) : text_(L"The game has to close: " + widen(error.what())) {
  }

void ErrorReport::show() const {
  MessageBoxW(nullptr, text_.c_str(), L"Quest for the Unicorn", MB_ICONERROR | MB_OK);
  }

}

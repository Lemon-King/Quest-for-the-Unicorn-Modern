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

#include <windows.h>

#include <stdexcept>

namespace platform {

/* A failed Direct3D or DXGI call: which call, and its HRESULT */
class Direct3DError : public std::runtime_error {
  public:
    Direct3DError(HRESULT result, const char* call);
    HRESULT result() const;
    bool deviceLost() const;

    /* Throws when FAILED(result) */
    static void check(HRESULT result, const char* call);

  private:
    HRESULT result_;
  };

}

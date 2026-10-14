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

#include <cstdint>

namespace platform {

/* Ticks at a fixed rate on a schedule kept from its start, so a late tick never shifts the ones after it. Its timer is
   signalled while a tick is due, for a thread to wait on beside its messages. */
class FrameClock {
  public:
    explicit FrameClock(int32_t ticksPerSecond);
    ~FrameClock();

    FrameClock(const FrameClock&) = delete;
    FrameClock& operator=(const FrameClock&) = delete;

    HANDLE due() const;
    void advance();

  private:
    LONGLONG tickTime(uint64_t tick) const;

    HANDLE   timer_;
    int32_t  ticksPerSecond_;
    LONGLONG frequency_;
    LONGLONG start_;
    uint64_t tick_;
  };

}

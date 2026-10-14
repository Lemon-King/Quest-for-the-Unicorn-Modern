/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "FrameClock.h"

#include <algorithm>
#include <stdexcept>

namespace platform {

/* The first tick is due one period from now */
FrameClock::FrameClock(int32_t ticksPerSecond)
  : timer_(nullptr), ticksPerSecond_(ticksPerSecond), frequency_(0), start_(0), tick_(0) {
  LARGE_INTEGER counter;

  if (ticksPerSecond < 1) throw std::invalid_argument("the frame clock needs a positive rate");

  /* A high-resolution timer (Windows 10 1803 on) keeps the rate; older systems fall back to their timer tick */
  timer_ = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
  if (timer_ == nullptr) timer_ = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
  if (timer_ == nullptr) throw std::runtime_error("CreateWaitableTimerExW failed");

  QueryPerformanceFrequency(&counter);
  frequency_ = counter.QuadPart;
  QueryPerformanceCounter(&counter);
  start_ = counter.QuadPart;

  /* No destructor runs for a constructor that throws, so the timer goes here */
  try {
    advance();
    }
  catch (...) {
    CloseHandle(timer_);
    throw;
    }
  }

FrameClock::~FrameClock() {
  CancelWaitableTimer(timer_);
  CloseHandle(timer_);
  }

HANDLE FrameClock::due() const {
  return timer_;
  }

/* Arms the timer for the next tick still ahead: ticks missed while late are dropped, not run back to back */
void FrameClock::advance() {
  LARGE_INTEGER now;
  LARGE_INTEGER due;
  uint64_t      passed;

  QueryPerformanceCounter(&now);
  passed = static_cast<uint64_t>((now.QuadPart - start_) * ticksPerSecond_ / frequency_);
  tick_ = std::max(tick_ + 1, passed + 1);

  /* A relative due time is negative, in 100 ns units */
  due.QuadPart = -((tickTime(tick_) - now.QuadPart) * 10000000 / frequency_);
  if (!SetWaitableTimer(timer_, &due, 0, nullptr, nullptr, FALSE)) throw std::runtime_error("SetWaitableTimer failed");
  }

/* In performance counter units */
LONGLONG FrameClock::tickTime(uint64_t tick) const {
  return start_ + static_cast<LONGLONG>(tick) * frequency_ / ticksPerSecond_;
  }

}

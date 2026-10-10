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

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>

namespace platform {

/* Keys on their way from the window to the game, and whether the window was closed. Once closed, peek(), pop() and
   waitPop() answer HOST_KEY_CLOSED even with keys waiting, and later keys are dropped. */
class InputQueue {
  public:
    void push(int32_t key);
    void close();
    int32_t peek() const;
    int32_t pop();
    int32_t waitPop();
    bool closed() const;

  private:
    mutable std::mutex      mutex_;
    std::condition_variable ready_;
    std::deque<int32_t>     keys_;
    bool                    closed_ = false;
  };

}

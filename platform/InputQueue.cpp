/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "InputQueue.h"

#include "platform.h"

namespace platform {

/* The waiting reader is woken after the lock is released, so it does not wake only to block on it */
void InputQueue::push(int32_t key) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (closed_) return;
  keys_.push_back(key);
  lock.unlock();
  ready_.notify_one();
  }

/* Wakes every waitPop(), so a game blocked on a key sees the close */
void InputQueue::close() {
  std::unique_lock<std::mutex> lock(mutex_);
  closed_ = true;
  lock.unlock();
  ready_.notify_all();
  }

int32_t InputQueue::peek() const {
  std::lock_guard<std::mutex> lock(mutex_);
  if (closed_) return HOST_KEY_CLOSED;
  return keys_.empty() ? HOST_KEY_NONE : keys_.front();
  }

int32_t InputQueue::pop() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (closed_) return HOST_KEY_CLOSED;
  if (keys_.empty()) return HOST_KEY_NONE;
  int32_t key = keys_.front();
  keys_.pop_front();
  return key;
  }

/* Blocks until a key arrives or the window closes */
int32_t InputQueue::waitPop() {
  std::unique_lock<std::mutex> lock(mutex_);
  ready_.wait(lock, [this] { return closed_ || !keys_.empty(); });
  if (closed_) return HOST_KEY_CLOSED;

  int32_t key = keys_.front();
  keys_.pop_front();
  return key;
  }

bool InputQueue::closed() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return closed_;
  }

}

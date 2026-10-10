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

void InputQueue::push(int32_t key) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!closed_) keys_.push_back(key);
  }

void InputQueue::close() {
  std::lock_guard<std::mutex> lock(mutex_);
  closed_ = true;
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

bool InputQueue::closed() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return closed_;
  }

}

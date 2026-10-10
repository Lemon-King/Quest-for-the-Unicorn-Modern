/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Keys come out in the order they went in, a closed window wins over waiting keys, and a blocked read wakes for
   either */

#include <chrono>
#include <string>
#include <thread>

#include "InputQueue.h"
#include "TestRunner.h"
#include "platform.h"

namespace {

void expect(TestRunner& runner, int32_t got, int32_t expected, const std::string& step) {
  runner.check(got == expected, "InputQueueOrder", step + " gave " + std::to_string(got) + ", expected " + std::to_string(expected));
  }

void inputQueueOrder(TestRunner& runner) {
  platform::InputQueue input;
  input.push(1);
  input.push(2);
  expect(runner, input.peek(), 1, "peek");
  expect(runner, input.pop(), 1, "first pop");
  expect(runner, input.pop(), 2, "second pop");
  expect(runner, input.peek(), HOST_KEY_NONE, "peek when empty");
  expect(runner, input.pop(), HOST_KEY_NONE, "pop when empty");
  runner.check(!input.closed(), "InputQueueOrder", "the queue should not start closed");

  input.push(3);
  input.close();
  expect(runner, input.peek(), HOST_KEY_CLOSED, "peek after close with a key waiting");
  input.push(4);
  expect(runner, input.pop(), HOST_KEY_CLOSED, "pop after close and a later push");
  expect(runner, input.pop(), HOST_KEY_CLOSED, "pop again after close");
  runner.check(input.closed(), "InputQueueOrder", "the queue should report closed");
  }

/* The game thread blocks in waitPop() until the window thread pushes a key or closes */
void inputQueueWaitPop(TestRunner& runner) {
  platform::InputQueue keyed;
  std::thread          pusher([&keyed] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    keyed.push(7);
    });
  int32_t key = keyed.waitPop();
  pusher.join();
  runner.check(key == 7, "InputQueueWaitPop", "a key pushed from another thread gave " + std::to_string(key));

  platform::InputQueue closing;
  std::thread          closer([&closing] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    closing.close();
    });
  int32_t closed = closing.waitPop();
  closer.join();
  runner.check(closed == HOST_KEY_CLOSED, "InputQueueWaitPop",
               "a close from another thread gave " + std::to_string(closed) + ", expected HOST_KEY_CLOSED");
  }

}

void runInputQueueTests(TestRunner& runner) {
  inputQueueOrder(runner);
  inputQueueWaitPop(runner);
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Frames reach the presenter whole at first, then only by what changed */

#include <string>

#include "Canvas.h"
#include "FrameMailbox.h"
#include "TestRunner.h"

namespace {

constexpr int32_t FRAME_SIZE = 16;
constexpr uint32_t COLOR = 0x123456;

/* What one collect() handed over: whether it called at all, the area, and the pixel at (5,6) */
struct Collected {
  bool             called = false;
  platform::Rect   area{0, 0, 0, 0};
  uint32_t         pixel = 0;
  };

Collected collect(platform::FrameMailbox& frames) {
  Collected result;
  frames.collect([&result](const uint32_t* pixels, int32_t stride, const platform::Rect& area) {
    result.called = true;
    result.area = area;
    result.pixel = pixels[6 * stride + 5];
    });
  return result;
  }

bool covers(const platform::Rect& area, int32_t left, int32_t top, int32_t right, int32_t bottom) {
  return area.left == left && area.top == top && area.right == right && area.bottom == bottom;
  }

std::string describe(const platform::Rect& area) {
  return "{" + std::to_string(area.left) + "," + std::to_string(area.top) + "," + std::to_string(area.right) + "," +
         std::to_string(area.bottom) + "}";
  }

void frameMailboxHandsOverChanges(TestRunner& runner) {
  platform::FrameMailbox frames(FRAME_SIZE, FRAME_SIZE);
  platform::Canvas       canvas(FRAME_SIZE, FRAME_SIZE);

  Collected first = collect(frames);
  runner.check(first.called && covers(first.area, 0, 0, FRAME_SIZE, FRAME_SIZE) && first.pixel == 0,
               "FrameMailboxHandsOverChanges",
               "the first collect should hand over the whole black frame, gave " + describe(first.area));

  runner.check(!frames.publish(canvas), "FrameMailboxHandsOverChanges", "an unchanged canvas should publish nothing");

  canvas.point(5, 6, COLOR);
  runner.check(frames.publish(canvas), "FrameMailboxHandsOverChanges", "a changed canvas should publish");
  Collected changed = collect(frames);
  runner.check(changed.called && covers(changed.area, 5, 6, 6, 7) && changed.pixel == COLOR, "FrameMailboxHandsOverChanges",
               "after drawing (5,6) the collect should hand over {5,6,6,7} with its colour, gave " + describe(changed.area));

  Collected again = collect(frames);
  runner.check(!again.called, "FrameMailboxHandsOverChanges", "a second collect should hand over nothing");

  frames.invalidate();
  Collected whole = collect(frames);
  runner.check(whole.called && covers(whole.area, 0, 0, FRAME_SIZE, FRAME_SIZE) && whole.pixel == COLOR,
               "FrameMailboxHandsOverChanges",
               "after invalidate the collect should hand over the whole frame, gave " + describe(whole.area));
  }

}

void runFrameMailboxTests(TestRunner& runner) {
  frameMailboxHandsOverChanges(runner);
  }

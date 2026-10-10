/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The canvas's place in the client area, as graphics.c's _rcs_layoutViewport worked it out */

#include <string>

#include "TestRunner.h"
#include "ViewportLayout.h"

namespace {

std::string describe(const platform::Placement& placement) {
  return "{" + std::to_string(placement.left) + "," + std::to_string(placement.top) + "," +
         std::to_string(placement.width) + "," + std::to_string(placement.height) + "}";
  }

void viewportLayoutPlaces(TestRunner& runner) {
  struct Case { int32_t clientWidth, clientHeight; platform::Placement expected; };
  const Case cases[] = {
    {640, 480, {0, 0, 640, 480}},         /* 1:1 */
    {1280, 960, {0, 0, 1280, 960}},       /* 2x */
    {1500, 1000, {83, 0, 1333, 1000}},    /* fractional, bars left and right */
    {600, 400, {-20, -40, 640, 480}},     /* smaller than the canvas: 1:1, centred and cropped */
    };

  platform::ViewportLayout layout(640, 480);
  for (const Case& item : cases) {
    platform::Placement placement = layout.place(item.clientWidth, item.clientHeight);
    bool same = placement.left == item.expected.left && placement.top == item.expected.top &&
                placement.width == item.expected.width && placement.height == item.expected.height;
    runner.check(same, "ViewportLayoutPlaces", std::to_string(item.clientWidth) + "x" + std::to_string(item.clientHeight) +
                 " gave " + describe(placement) + ", expected " + describe(item.expected));
    }
  }

}

void runViewportLayoutTests(TestRunner& runner) {
  viewportLayoutPlaces(runner);
  }

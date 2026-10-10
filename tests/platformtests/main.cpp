/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Conformance tests for the platform layer: each part against the GDI behaviour it replaces */

#include "TestRunner.h"

void runCanvasTests(TestRunner& runner);
void runSpriteTests(TestRunner& runner);
void runBmpWriterTests(TestRunner& runner);
void runViewportLayoutTests(TestRunner& runner);
void runFrameMailboxTests(TestRunner& runner);
void runInputQueueTests(TestRunner& runner);
void runKeyTranslatorTests(TestRunner& runner);
void runScalePassTests(TestRunner& runner);

int main() {
  TestRunner runner;
  runCanvasTests(runner);
  runSpriteTests(runner);
  runBmpWriterTests(runner);
  runViewportLayoutTests(runner);
  runFrameMailboxTests(runner);
  runInputQueueTests(runner);
  runKeyTranslatorTests(runner);
  runScalePassTests(runner);
  return runner.finish();
  }

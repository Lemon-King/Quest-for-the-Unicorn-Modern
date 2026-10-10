/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "TestRunner.h"

#include <algorithm>
#include <cstdio>

namespace {

/* A test that fails thousands of checks would bury the summary; the count still includes them all */
constexpr int PRINTED_FAILURES = 25;

}

void TestRunner::check(bool passed, const std::string& test, const std::string& detail) {
  if (passed) {
    passed_++;
    return;
    }

  failed_++;
  if (failed_ <= PRINTED_FAILURES) std::printf("FAIL %s: %s\n", test.c_str(), detail.c_str());
  if (failed_ == PRINTED_FAILURES + 1) std::printf("(further failures not printed)\n");
  }

void TestRunner::skip(const std::string& test, const std::string& reason) {
  std::printf("SKIP %s: %s\n", test.c_str(), reason.c_str());
  }

int TestRunner::finish() const {
  std::printf("platformtests: %d passed, %d failed\n", passed_, failed_);
  return std::min(failed_, 255);
  }

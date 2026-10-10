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
#include <memory>

#include "FrameMailbox.h"
#include "InputQueue.h"
#include "KeyTranslator.h"
#include "Presenter.h"
#include "ViewportLayout.h"

namespace platform {

/* The game's top-level window: opens at the sharpest size that fits, repaints from the newest frame whenever Windows
   asks, turns key messages into game keys, and reports a close to the game */
class GameWindow {
  public:
    GameWindow(HINSTANCE instance, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval, FrameMailbox& frames,
               InputQueue& input);
    ~GameWindow();

    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    HWND handle() const;
    void showFrame();

  private:
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT handle(UINT message, WPARAM wParam, LPARAM lParam);
    int32_t openingScale() const;
    SIZE frameSize(int32_t clientWidth, int32_t clientHeight) const;
    void setClientSize(int32_t clientWidth, int32_t clientHeight);
    void limitSize(MINMAXINFO* limits) const;
    void destroy();

    HWND                       window_;
    int32_t                    canvasWidth_;
    int32_t                    canvasHeight_;
    FrameMailbox&              frames_;
    InputQueue&                input_;
    KeyTranslator              keys_;
    ViewportLayout             layout_;
    Placement                  placement_;
    std::unique_ptr<Presenter> presenter_;
  };

}

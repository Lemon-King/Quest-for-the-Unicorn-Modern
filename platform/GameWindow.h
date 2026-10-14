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

#include <atomic>
#include <cstdint>
#include <memory>

#include "FrameClock.h"
#include "FrameMailbox.h"
#include "InputQueue.h"
#include "KeyTranslator.h"
#include "Presenter.h"
#include "ViewportLayout.h"

namespace platform {

constexpr UINT WM_APP_FRAME = WM_APP + 1;        /* a frame was published: present it */
constexpr UINT WM_APP_GAME_ENDED = WM_APP + 2;   /* run_game() returned: end the message loop */
constexpr UINT WM_APP_TICK = WM_APP + 3;         /* the frame clock ticked */

/* The game's top-level window: opens at the sharpest size that fits, repaints from the newest frame whenever Windows
   asks and at least 60 times a second, turns key messages into game keys, and reports a close to the game */
class GameWindow {
  public:
    GameWindow(HINSTANCE instance, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval, bool integerScaling,
               FrameMailbox& frames, InputQueue& input);
    ~GameWindow();

    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    HWND handle() const;
    void showFrame();
    void requestFrame();
    void gameEnded();
    bool translate(const MSG& message);
    void toggleFullscreen();
    void awaitMessage();
    void presentIfDue();

  private:
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT handle(UINT message, WPARAM wParam, LPARAM lParam);
    void presentTicks(DWORD timeout);
    bool route(const KeyRoute& route);
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
    FrameClock                 clock_;
    bool                       shownSinceTick_ = false;
    std::atomic<bool>          framePending_ = false;
    bool                       fullscreen_ = false;
    WINDOWPLACEMENT            restore_ = { sizeof(WINDOWPLACEMENT) };
  };

}

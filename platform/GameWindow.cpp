/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "GameWindow.h"

#include <optional>
#include <stdexcept>

#include "ErrorReport.h"

namespace platform {

namespace {

constexpr wchar_t CLASS_NAME[] = L"QuestForTheUnicorn";

}

GameWindow::GameWindow(HINSTANCE instance, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval,
                       FrameMailbox& frames, InputQueue& input)
  : window_(nullptr), canvasWidth_(canvasWidth), canvasHeight_(canvasHeight), frames_(frames), input_(input), keys_(),
    layout_(canvasWidth, canvasHeight), placement_{0, 0, canvasWidth, canvasHeight}, presenter_() {
  WNDCLASSEXW windowClass = {};
  windowClass.cbSize = sizeof(windowClass);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = procedure;
  windowClass.hInstance = instance;
  windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  windowClass.lpszClassName = CLASS_NAME;
  if (RegisterClassExW(&windowClass) == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
    throw std::runtime_error("RegisterClassExW failed");

  /* WM_NCCREATE sets window_ */
  if (CreateWindowExW(0, CLASS_NAME, L"Quest for the Unicorn", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                      canvasWidth, canvasHeight, nullptr, nullptr, instance, this) == nullptr)
    throw std::runtime_error("CreateWindowExW failed");

  /* No destructor runs for a constructor that throws, so the window goes here */
  try {
    RECT    client;
    int32_t scale = openingScale();
    setClientSize(canvasWidth * scale, canvasHeight * scale);
    GetClientRect(window_, &client);
    presenter_ = std::make_unique<Presenter>(window_, canvasWidth, canvasHeight, syncInterval);
    placement_ = layout_.place(client.right, client.bottom);
    ShowWindow(window_, SW_SHOW);
    }
  catch (...) {
    destroy();
    throw;
    }
  }

GameWindow::~GameWindow() {
  destroy();
  }

HWND GameWindow::handle() const {
  return window_;
  }

/* Presents the newest frame, as published to the mailbox */
void GameWindow::showFrame() {
  if (presenter_ != nullptr) presenter_->present(frames_, placement_);
  }

/* Safe from any thread; frames published while one is pending are shown together */
void GameWindow::requestFrame() {
  if (!framePending_.exchange(true)) PostMessageW(window_, WM_APP_FRAME, 0, 0);
  }

/* Safe from any thread */
void GameWindow::gameEnded() {
  PostMessageW(window_, WM_APP_GAME_ENDED, 0, 0);
  }

LRESULT CALLBACK GameWindow::procedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  GameWindow* self;
  if (message == WM_NCCREATE) {
    self = static_cast<GameWindow*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
    self->window_ = window;
    SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }

  self = reinterpret_cast<GameWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
  if (self == nullptr) return DefWindowProcW(window, message, wParam, lParam);

  /* Nothing may unwind through Windows' own frames */
  try {
    return self->handle(message, wParam, lParam);
    }
  catch (const std::exception& error) {
    ErrorReport(error).show();
    ExitProcess(1);
    }
  }

LRESULT GameWindow::handle(UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT paint;
  RECT*       suggested;

  switch (message) {
    case WM_SIZE:
      if (presenter_ != nullptr && wParam != SIZE_MINIMIZED) {
        presenter_->resize(LOWORD(lParam), HIWORD(lParam));
        placement_ = layout_.place(LOWORD(lParam), HIWORD(lParam));
        }
      return 0;

    case WM_PAINT:
      BeginPaint(window_, &paint);
      EndPaint(window_, &paint);
      showFrame();
      return 0;

    case WM_ERASEBKGND:
      return 1;

    case WM_GETMINMAXINFO:
      limitSize(reinterpret_cast<MINMAXINFO*>(lParam));
      return 0;

    case WM_DPICHANGED:
      suggested = reinterpret_cast<RECT*>(lParam);
      SetWindowPos(window_, nullptr, suggested->left, suggested->top, suggested->right - suggested->left,
                   suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);
      return 0;

    case WM_KEYDOWN:
      if (std::optional<int32_t> key = keys_.keyDown(wParam, GetKeyState(VK_CAPITAL) != 0)) input_.push(*key);
      return 0;

    case WM_KEYUP:
      keys_.keyUp(wParam);
      return 0;

    case WM_KILLFOCUS:
      keys_.reset();
      return 0;

    /* The game ends at its next key read or delay, without saving, as the close button always did */
    case WM_CLOSE:
      ShowWindow(window_, SW_HIDE);
      input_.close();
      return 0;

    case WM_APP_FRAME:
      framePending_ = false;
      showFrame();
      return 0;

    case WM_APP_GAME_ENDED:
      PostQuitMessage(0);
      return 0;
    }

  return DefWindowProcW(window_, message, wParam, lParam);
  }

/* The whole-number scale nearest the monitor's DPI at which the window still fits its work area */
int32_t GameWindow::openingScale() const {
  MONITORINFO monitor = {};
  int32_t     scale;
  monitor.cbSize = sizeof(monitor);
  GetMonitorInfoW(MonitorFromWindow(window_, MONITOR_DEFAULTTOPRIMARY), &monitor);
  scale = static_cast<int32_t>((GetDpiForWindow(window_) + USER_DEFAULT_SCREEN_DPI / 2) / USER_DEFAULT_SCREEN_DPI);

  while (scale > 1) {
    SIZE outer = frameSize(canvasWidth_ * scale, canvasHeight_ * scale);
    if (outer.cx <= monitor.rcWork.right - monitor.rcWork.left && outer.cy <= monitor.rcWork.bottom - monitor.rcWork.top)
      break;
    scale--;
    }
  return (scale < 1) ? 1 : scale;
  }

/* Outer window size for a client area, with the frame at the window's DPI */
SIZE GameWindow::frameSize(int32_t clientWidth, int32_t clientHeight) const {
  RECT frame = { 0, 0, clientWidth, clientHeight };
  AdjustWindowRectExForDpi(&frame, static_cast<DWORD>(GetWindowLongW(window_, GWL_STYLE)), FALSE,
                           static_cast<DWORD>(GetWindowLongW(window_, GWL_EXSTYLE)), GetDpiForWindow(window_));
  return SIZE{ frame.right - frame.left, frame.bottom - frame.top };
  }

void GameWindow::setClientSize(int32_t clientWidth, int32_t clientHeight) {
  SIZE outer = frameSize(clientWidth, clientHeight);
  SetWindowPos(window_, nullptr, 0, 0, outer.cx, outer.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
  }

/* Keeps the client area from shrinking below the canvas */
void GameWindow::limitSize(MINMAXINFO* limits) const {
  if (window_ == nullptr) return;
  SIZE outer = frameSize(canvasWidth_, canvasHeight_);
  limits->ptMinTrackSize.x = outer.cx;
  limits->ptMinTrackSize.y = outer.cy;
  }

/* Messages from here on go straight to Windows, never to a half-gone GameWindow */
void GameWindow::destroy() {
  if (window_ == nullptr) return;
  SetWindowLongPtrW(window_, GWLP_USERDATA, 0);
  DestroyWindow(window_);
  window_ = nullptr;
  }

}

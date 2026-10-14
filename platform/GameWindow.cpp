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

/* res/unicorn.rc: the unicorn sprite as the application icon */
constexpr int APPLICATION_ICON = 1;

/* The window shows a frame this often even while the game draws nothing, so screen recorders keep getting frames */
constexpr int32_t BASE_FRAME_RATE = 60;

}

GameWindow::GameWindow(HINSTANCE instance, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval,
                       bool integerScaling, FrameMailbox& frames, InputQueue& input)
  : window_(nullptr), canvasWidth_(canvasWidth), canvasHeight_(canvasHeight), frames_(frames), input_(input), keys_(),
    layout_(canvasWidth, canvasHeight, integerScaling), placement_{0, 0, canvasWidth, canvasHeight}, presenter_(),
    clock_(BASE_FRAME_RATE) {
  WNDCLASSEXW windowClass = {};
  windowClass.cbSize = sizeof(windowClass);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = procedure;
  windowClass.hInstance = instance;
  windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  windowClass.lpszClassName = CLASS_NAME;

  /* Title bar and taskbar; without the resource (the tests' executable) both stay null, the default icon */
  windowClass.hIcon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(APPLICATION_ICON), IMAGE_ICON,
                                                    GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0));
  windowClass.hIconSm = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(APPLICATION_ICON), IMAGE_ICON,
                                                      GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0));
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
  shownSinceTick_ = true;
  }

/* Blocks until a message waits in this thread's queue, presenting at each tick of the frame clock meanwhile */
void GameWindow::awaitMessage() {
  presentTicks(INFINITE);
  }

/* Presents a tick of the frame clock that is already due, without waiting */
void GameWindow::presentIfDue() {
  presentTicks(0);
  }

/* Safe from any thread; frames published while one is pending are shown together */
void GameWindow::requestFrame() {
  if (!framePending_.exchange(true)) PostMessageW(window_, WM_APP_FRAME, 0, 0);
  }

/* Safe from any thread */
void GameWindow::gameEnded() {
  PostMessageW(window_, WM_APP_GAME_ENDED, 0, 0);
  }

/* Key messages for this window, before TranslateMessage: true when they were dealt with here */
bool GameWindow::translate(const MSG& message) {
  if (message.hwnd != window_) return false;

  switch (message.message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      return route(keys_.keyDown(message.message, message.wParam, message.lParam));

    case WM_KEYUP:
    case WM_SYSKEYUP:
      keys_.keyUp(message.wParam);

      /* Releasing Alt or F10 would otherwise put the window into menu mode */
      return message.wParam == VK_MENU || message.wParam == VK_F10;
    }
  return false;
  }

/* Borderless over the whole monitor the window is on, then back to exactly where the window was. A popup has no
   frame, so the minimum size lets it cover the monitor, and WM_SIZE resizes the swap chain as for any size. The old
   window's pixels are discarded rather than kept across the move, and the frame is shown at once at the new size:
   Windows 10 could otherwise leave the old caption and borders on screen until the game next drew. */
void GameWindow::toggleFullscreen() {
  MONITORINFO monitor = { sizeof(MONITORINFO) };

  if (!fullscreen_) {
    GetWindowPlacement(window_, &restore_);
    GetMonitorInfoW(MonitorFromWindow(window_, MONITOR_DEFAULTTONEAREST), &monitor);
    SetWindowLongPtrW(window_, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(window_, HWND_TOP, monitor.rcMonitor.left, monitor.rcMonitor.top,
                 monitor.rcMonitor.right - monitor.rcMonitor.left, monitor.rcMonitor.bottom - monitor.rcMonitor.top,
                 SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOOWNERZORDER);
    fullscreen_ = true;
    }
  else {
    SetWindowLongPtrW(window_, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    SetWindowPlacement(window_, &restore_);
    SetWindowPos(window_, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    fullscreen_ = false;
    }

  showFrame();
  }

bool GameWindow::route(const KeyRoute& route) {
  switch (route.target()) {
    case KeyRoute::Target::Game:
      input_.push(route.key());
      return true;

    case KeyRoute::Target::Swallow:
      return true;

    case KeyRoute::Target::Fullscreen:
      toggleFullscreen();
      return true;

    case KeyRoute::Target::Windows:
      return false;
    }
  return false;
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

    case WM_CHAR:
      if (std::optional<int32_t> key = keys_.character(wParam)) input_.push(*key);
      return 0;

    /* Alt+letter: no beep, nothing for the game. Alt+Space goes on, as DefWindowProc opens the system menu from it */
    case WM_SYSCHAR:
      if (wParam == ' ') break;
      return 0;

    /* Plain Alt or F10 must not open the system menu; Alt+Space still does */
    case WM_SYSCOMMAND:
      if ((wParam & 0xfff0) == SC_KEYMENU && lParam == 0) return 0;
      break;

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

    /* The frame again, unless the game has shown one since the tick before */
    case WM_APP_TICK:
      clock_.advance();
      if (!shownSinceTick_) showFrame();
      shownSinceTick_ = false;
      return 0;

    case WM_APP_GAME_ENDED:
      PostQuitMessage(0);
      return 0;
    }

  return DefWindowProcW(window_, message, wParam, lParam);
  }

/* Ticks go through the window procedure, which reports a failure as it does for every message; a waiting message ends
   the wait */
void GameWindow::presentTicks(DWORD timeout) {
  HANDLE due = clock_.due();

  while (MsgWaitForMultipleObjectsEx(1, &due, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0) {
    SendMessageW(window_, WM_APP_TICK, 0, 0);
    }
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

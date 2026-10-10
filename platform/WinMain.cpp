/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The composition root: the game's options first, then the window, its frames and keys, and the host that runs the
   game in it */

#include <windows.h>

#include <cstdlib>
#include <exception>

#include "ErrorReport.h"
#include "FrameMailbox.h"
#include "GameWindow.h"
#include "InlineHost.h"
#include "InputQueue.h"
#include "ThreadedHost.h"
#include "platform.h"

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
  GAME_SETUP setup = {};

  /* The CRT splits the command line; with the UTF-8 code page from the manifest, __argv is UTF-8 */
  if (configure_game(__argc, __argv, &setup) == 0) return 0;

  try {
    platform::FrameMailbox frames(setup.width, setup.height);
    platform::InputQueue   input;
    platform::GameWindow   window(instance, setup.width, setup.height, setup.gameThread ? 1 : 0, frames, input);

    /* GameThread: the window thread presents with vsync. NoGameThread: as the GDI build, never waiting for vblank */
    if (setup.gameThread) {
      platform::ThreadedHost host(frames, input, window);
      return host.run();
      }

    platform::InlineHost host(frames, input, window);
    return host.run();
    }
  catch (const std::exception& error) {
    platform::ErrorReport(error).show();
    return 1;
    }
  }

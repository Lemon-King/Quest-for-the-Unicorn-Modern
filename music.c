/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 * Background music. Each song is a MIDI file in the music folder beside the game, played in a loop as the DOS
 * release's CMSDRV played its songs. Windows plays them through the MCI sequencer. A hidden message-only window
 * restarts a song that reaches its end; its messages arrive through the game's own message loop. Other builds have
 * no music.
 */

#include "header.h"

#ifdef WIN32
#include <mmsystem.h>

#define MUSIC_ALIAS  "unicornmusic"
#define MUSIC_CLASS  "unicornmusic"
#define MUSIC_COUNT  ((Int32)(sizeof(MUSIC_FILES) / sizeof(MUSIC_FILES[0])))

/* One MIDI file per song, indexed by the MUSIC_ numbers */
static char* MUSIC_FILES[] = {
  "music\\intro.mid",
  "music\\upland.mid",
  "music\\dungeon.mid",
  "music\\town.mid"
  };

static HWND listener = NULL;

static LRESULT CALLBACK music_listener(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
  if (message != MM_MCINOTIFY) return DefWindowProc(hWnd,message,wParam,lParam);

  /* A song that played to its end starts over; stopping or replacing it aborts instead */
  if (wParam == MCI_NOTIFY_SUCCESSFUL && (MCIDEVICEID)lParam == mciGetDeviceID(MUSIC_ALIAS))
    mciSendString("play " MUSIC_ALIAS " from 0 notify",NULL,0,hWnd);
  return 0;
  }

static HWND music_window() {
  WNDCLASSEX wcex;

  if (listener != NULL) return listener;

  memset(&wcex,0,sizeof(wcex));
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpfnWndProc = music_listener;
  wcex.hInstance = GetModuleHandle(NULL);
  wcex.lpszClassName = MUSIC_CLASS;
  RegisterClassEx(&wcex);

  listener = CreateWindowEx(0,MUSIC_CLASS,"",0,0,0,0,0,HWND_MESSAGE,NULL,wcex.hInstance,NULL);
  return listener;
  }
#endif

void stop_music() {
#ifdef WIN32
  mciSendString("close " MUSIC_ALIAS,NULL,0,NULL);
#endif
  }

void play_music(Int32 song) {
#ifdef WIN32
  char command[MAX_PATH + 64];
  HWND window;

  if (music != 'Y' || song < 0 || song >= MUSIC_COUNT) return;

  stop_music();
  snprintf(command,sizeof(command),"open \"%s\" type sequencer alias " MUSIC_ALIAS,MUSIC_FILES[song]);
  if (mciSendString(command,NULL,0,NULL) != 0) return;

  window = music_window();
  if (window != NULL) mciSendString("play " MUSIC_ALIAS " from 0 notify",NULL,0,window);
  else mciSendString("play " MUSIC_ALIAS " from 0",NULL,0,NULL);
#endif
  }

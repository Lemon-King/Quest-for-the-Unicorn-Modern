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
 * release's CMSDRV played its songs. Windows plays them through the MCI sequencer.
 *
 * Opening a song takes MCI about a second, so all MCI work happens on a music thread with its own message-only
 * window. The game only posts requests to that window and never waits. Each song is opened once and kept open, as
 * DOS kept all four in EMS; after the first song starts, the thread opens the others in the background. Other builds
 * have no music.
 */

#include "header.h"

#ifdef WIN32
#include <mmsystem.h>

#define MUSIC_CLASS    "unicornmusic"
#define MUSIC_COUNT    ((Int32)(sizeof(MUSIC_FILES) / sizeof(MUSIC_FILES[0])))
#define WM_MUSIC_PLAY  (WM_APP + 1)
#define WM_MUSIC_STOP  (WM_APP + 2)

/* One MIDI file and MCI alias per song, indexed by the MUSIC_ numbers */
static char* MUSIC_FILES[] = {
  "music\\intro.mid",
  "music\\upland.mid",
  "music\\dungeon.mid",
  "music\\town.mid"
  };

static char* MUSIC_ALIASES[] = {
  "unicornintro",
  "unicornupland",
  "unicorndungeon",
  "unicorntown"
  };

/* Game thread: the music thread's window, and the song the game last asked for */
static HWND  music_window = NULL;
static Int32 requested = -1;

/* Music thread: which songs MCI has open, and which one is playing */
static char  opened[MUSIC_COUNT];
static Int32 playing = -1;

static void mci_command(char* verb,Int32 song,char* options,HWND notify) {
  char command[MAX_PATH + 64];
  snprintf(command,sizeof(command),"%s %s %s",verb,MUSIC_ALIASES[song],options);
  mciSendString(command,NULL,0,notify);
  }

static char open_song(Int32 song) {
  char command[MAX_PATH + 64];
  if (opened[song] == 'Y') return 'Y';
  snprintf(command,sizeof(command),"open \"%s\" type sequencer alias %s",MUSIC_FILES[song],MUSIC_ALIASES[song]);
  opened[song] = (mciSendString(command,NULL,0,NULL) == 0) ? 'Y' : 'N';
  return opened[song];
  }

static void stop_song() {
  if (playing >= 0) mci_command("stop",playing,"",NULL);
  playing = -1;
  }

static void start_song(HWND window,Int32 song) {
  Int32 i;

  stop_song();
  if (open_song(song) != 'Y') return;

  mci_command("play",song,"from 0 notify",window);
  playing = song;

  for (i=0; i<MUSIC_COUNT; i++) open_song(i);
  }

static LRESULT CALLBACK music_events(HWND window,UINT message,WPARAM wParam,LPARAM lParam) {
  switch (message) {
    case WM_MUSIC_PLAY:
      start_song(window,(Int32)wParam);
      return 0;

    case WM_MUSIC_STOP:
      stop_song();
      return 0;

    /* A song that played to its end starts over; stopping it aborts instead */
    case MM_MCINOTIFY:
      if (wParam == MCI_NOTIFY_SUCCESSFUL && playing >= 0 &&
          (MCIDEVICEID)lParam == mciGetDeviceID(MUSIC_ALIASES[playing]))
        mci_command("play",playing,"from 0 notify",window);
      return 0;
    }
  return DefWindowProc(window,message,wParam,lParam);
  }

static DWORD WINAPI music_thread(LPVOID ready) {
  WNDCLASSEX wcex;
  MSG        msg;

  memset(&wcex,0,sizeof(wcex));
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpfnWndProc = music_events;
  wcex.hInstance = GetModuleHandle(NULL);
  wcex.lpszClassName = MUSIC_CLASS;
  RegisterClassEx(&wcex);

  music_window = CreateWindowEx(0,MUSIC_CLASS,"",0,0,0,0,0,HWND_MESSAGE,NULL,wcex.hInstance,NULL);
  SetEvent((HANDLE)ready);
  if (music_window == NULL) return 0;

  while (GetMessage(&msg,NULL,0,0) > 0) DispatchMessage(&msg);
  return 0;
  }

/* Starts the music thread on first use and waits only until its window exists */
static HWND music_thread_window() {
  static char started = 'N';
  HANDLE      ready;
  HANDLE      thread;

  if (started == 'Y') return music_window;
  started = 'Y';

  ready = CreateEvent(NULL,TRUE,FALSE,NULL);
  if (ready == NULL) return NULL;
  thread = CreateThread(NULL,0,music_thread,ready,0,NULL);
  if (thread != NULL) {
    WaitForSingleObject(ready,INFINITE);
    CloseHandle(thread);
    }
  CloseHandle(ready);
  return music_window;
  }
#endif

void play_music(Int32 song) {
#ifdef WIN32
  HWND window;

  if (song < 0 || song >= MUSIC_COUNT) return;
  requested = song;
  if (music != 'Y') return;

  window = music_thread_window();
  if (window != NULL) PostMessage(window,WM_MUSIC_PLAY,(WPARAM)song,0);
#endif
  }

void stop_music() {
#ifdef WIN32
  if (music_window != NULL) PostMessage(music_window,WM_MUSIC_STOP,0,0);
#endif
  }

/* Ctrl+M: silences the music, or brings back the song for where the player is */
void toggle_music() {
  music = (music == 'Y') ? 'N' : 'Y';
#ifdef WIN32
  if (music == 'Y' && requested >= 0) play_music(requested);
  if (music == 'N') stop_music();
#endif
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#define TERMINAL

#ifdef MINGW
#include <windows.h>
#include <conio.h>
#endif

#ifdef ASCII

#ifndef MINGW
#include <termios.h>
struct termios original;
#endif // MINGW

#include <stdio.h>

#else  // ASCII

#include "graphics.h"

/* Drawing goes to xscreen only; the viewport shows it in the window when the game flushes or waits */
rcs_Event key_event;
rcs_Window         window;
rcs_Display*       display;
rcs_Pixmap         xscreen;
rcs_Pixmap         screenSave;
rcs_Viewport       viewport;
rcs_GC             colors[16];
rcs_GC             gc;
#endif // ASCII

#include <sys/time.h>
#include <sys/timeb.h>

#include "header.h"
#include "terminal.h"

char screen[80][25][2];
char store[80][25][2];
Int32  esc_mode;
char esc_seq[80];
Int32 cursor_x;
Int32 cursor_y;
Int32 text_color;
Int32 g_color;

#ifndef ASCII
static void clear_buffer() {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_namedForeground(display,gc,"black");
  rcs_drawFilledBox(display,xscreen,gc,0,0,screen_width,screen_height);
  rcs_closeGC(display,gc);
  rcs_invalidateViewport(viewport,0,0,screen_width,screen_height);
  }

static void invalidate_string(rcs_GC gc,Int32 x,Int32 y,char* buffer) {
  Int32 left,top,width,height;
  rcs_stringBounds(display,gc,x,y,buffer,&left,&top,&width,&height);
  rcs_invalidateViewport(viewport,left,top,width,height);
  }
#endif

void hide_cursor() {
#ifdef MINGW
  HANDLE hConsole;
  CONSOLE_CURSOR_INFO cursor;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  cursor.dwSize = 100;
  cursor.bVisible = FALSE;
  SetConsoleCursorInfo(hConsole,&cursor);
#else
  printf("\E[?25l");
#endif
  }

void show_cursor() {
#ifdef MINGW
  HANDLE hConsole;
  CONSOLE_CURSOR_INFO cursor;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  cursor.dwSize = 100;
  cursor.bVisible = TRUE;
  SetConsoleCursorInfo(hConsole,&cursor);
#else
  printf("\E[?25h");
#endif
  }

void clrscr() {
#ifdef ASCII
  Int32 x,y;
  hide_cursor();
#ifdef MINGW
  HANDLE hConsole;
  COORD coordScreen = {0,0};
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwConSize;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hConsole,&csbi);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hConsole,(TCHAR)' ',dwConSize,coordScreen,&cCharsWritten);
  GetConsoleScreenBufferInfo(hConsole,&csbi);
  FillConsoleOutputAttribute(hConsole,csbi.wAttributes,dwConSize,coordScreen,&cCharsWritten);
  SetConsoleCursorPosition(hConsole,coordScreen);
#endif  // MINGW
  for (x=0; x<80; x++)
    for (y=0; y<25; y++) {
      screen[x][y][0] = ' ';
      screen[x][y][1] = 0;
      }
#ifndef MINGW
  printf("\E[H\E[2J\E[[?7l");
  if (no_color != 'Y') printf("\e[0;40m");
  printf("\E[H\E[2J\E[?7l");
  fflush(stdout);
#endif
#else
  clear_buffer();
#endif
  }

void position(Int32 x,Int32 y) {
#ifdef MINGW
  HANDLE hConsole;
  COORD coordScreen;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  coordScreen.X = x;
  coordScreen.Y = y;
  SetConsoleCursorPosition(hConsole,coordScreen);
#else
  printf("\e[%d;%dH",cursor_y+1,cursor_x+1);
#endif
  }

#ifdef ASCII
void gc_color(Int32 clr) {
#ifdef MINGW
  HANDLE h;
  h = GetStdHandle(STD_OUTPUT_HANDLE);
  if (no_color == 'Y') return;
  switch (clr) {
    case 0: SetConsoleTextAttribute(h,0); /* A black */ break; 
    case 1: SetConsoleTextAttribute(h,FOREGROUND_BLUE); /* A black */ break; 
    case 2: SetConsoleTextAttribute(h,FOREGROUND_GREEN); /* A black */ break; 
    case 3: SetConsoleTextAttribute(h,FOREGROUND_BLUE | FOREGROUND_GREEN); /* A black */ break; 
    case 4: SetConsoleTextAttribute(h,FOREGROUND_RED); /* A black */ break; 
    case 5: SetConsoleTextAttribute(h,FOREGROUND_BLUE | FOREGROUND_RED); /* A black */ break; 
    case 6: SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_RED); /* A black */ break; 
    case 7: SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); /* A black */ break; 

    case 8: SetConsoleTextAttribute(h,FOREGROUND_INTENSITY); /* A black */ break; 
    case 9: SetConsoleTextAttribute(h,FOREGROUND_BLUE | FOREGROUND_INTENSITY); /* A black */ break; 
    case 10: SetConsoleTextAttribute(h,FOREGROUND_GREEN | FOREGROUND_INTENSITY); /* A black */ break; 
    case 11: SetConsoleTextAttribute(h,FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY); /* A black */ break; 
    case 12: SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_INTENSITY); /* A black */ break; 
    case 13: SetConsoleTextAttribute(h,FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY); /* A black */ break; 
    case 14: SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); /* A black */ break; 
    case 15: SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); /* A black */ break; 

    }
#else
  switch (clr) {
    case 0:printf("\e[0;30;40m"); /* A black */ break; 
    case 1:printf("\e[0;34;40m"); /* B blue */ break;
    case 2:printf("\e[0;32;40m"); /* C green */ break;
    case 3:printf("\e[0;36;40m"); /* D cyan */ break;
    case 4:printf("\e[0;31;40m"); /* E red */ break;
    case 5:printf("\e[0;35;40m"); /* F purple */ break;
    case 6:printf("\e[0;33;40m"); /* G brown */ break;
    case 7:printf("\e[0;37;40m"); /* H gray */ break;
    case 8:printf("\e[1;30;40m"); /* I dark gray */ break;
    case 9:printf("\e[1;34;40m"); /* J light blue */ break;
    case 10:printf("\e[1;32;40m"); /* K light green */ break;
    case 11:printf("\e[1;36;40m"); /* L light cyan */ break;
    case 12:printf("\e[1;31;40m"); /* M light red */ break;
    case 13:printf("\e[1;35;40m"); /* N light purple */ break;
    case 14:printf("\e[1;33;40m"); /* O yellow */ break;
    case 15:printf("\e[1;37;40m"); /* P white */ break;
    }
#endif
  fflush(stdout);
  }
#else
void gc_color(rcs_GC* gc,Int32 clr) {
  switch (clr) {
    case 0:rcs_namedForeground(display,*gc,"black"); break;
    case 1:rcs_namedForeground(display,*gc,"blue3"); break;
    case 2:rcs_namedForeground(display,*gc,"green3"); break;
    case 3:rcs_namedForeground(display,*gc,"cyan"); break;
    case 4:rcs_namedForeground(display,*gc,"red"); break;
    case 5:rcs_namedForeground(display,*gc,"purple"); break;
    case 6:rcs_namedForeground(display,*gc,"brown"); break;
    case 7:rcs_namedForeground(display,*gc,"gray50"); break;
    case 8:rcs_namedForeground(display,*gc,"gray25"); break;
    case 9:rcs_namedForeground(display,*gc,"blue1"); break;
    case 10:rcs_namedForeground(display,*gc,"green1"); break;
    case 11:rcs_namedForeground(display,*gc,"light cyan"); break;
    case 12:rcs_namedForeground(display,*gc,"red1"); break;
    case 13:rcs_namedForeground(display,*gc,"purple1"); break;
    case 14:rcs_namedForeground(display,*gc,"yellow"); break;
    case 15:rcs_namedForeground(display,*gc,"white"); break;
    }
  rcs_namedBackground(display, *gc, "black");
  }
#endif

Int32 unbuffer() {
#ifdef ASCII
#ifndef MINGW
  struct termios terminal;
  tcgetattr(0,&terminal);
  original = terminal;
  terminal.c_lflag &= ~ICANON;
  terminal.c_lflag &= ~ECHO;
  if (tcsetattr(0,TCSANOW,&terminal)!=0) return -1;
  printf("\E[?25l");
  esc_mode = 0;
#endif  // MINGW
#endif  // ASCII
  return 0;
  }

#ifdef ASCII
void init_graph() {
  Int32 x,y;
  for (x=0; x<80; x++)
    for (y=0; y<25; y++) {
      screen[x][y][0] = ' ';
      screen[x][y][1] = 0;
      }
  clrscr();
  }
#else
void init_graph() {
  Int32 i;
  display = rcs_openDisplay(":0.0");
  rcs_buildColorMapping(display);
  window = rcs_createWindow(display,rcs_rootWindow(display),0,0,screen_width,screen_height);
  rcs_setWindowName(display,window,"Quest for the Unicorn");
  xscreen = rcs_createPixmap(display,window,screen_width,screen_height);
  screenSave = rcs_createPixmap(display,window,screen_width,screen_height);
  viewport = rcs_createViewport(display,window,xscreen,screen_width,screen_height);
  clear_buffer();
  rcs_showWindow(display,window);
  for (i=0; i<16; i++) {
    colors[i] = rcs_openGC(display,window);
    gc_color(&(colors[i]),i);
    }
  }
#endif

#ifdef ASCII
void saveScreen() {
  Int32 x,y;
  for (x=0; x<80; x++)
    for (y=0; y<25; y++) {
      store[x][y][0] = screen[x][y][0];
      store[x][y][1] = screen[x][y][1];
      }
  }
#else
void saveScreen() {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_copyArea(display,xscreen,screenSave,gc,0,0,screen_width,screen_height,0,0);
  rcs_closeGC(display,gc);
  }
#endif

#ifdef ASCII
void restoreScreen() {
  Int32 x,y;
  for (y=0; y<25; y++) {
    for (x=0; x<79; x++) {
      screen[x][y][0] = store[x][y][0];
      screen[x][y][1] = store[x][y][1];
      gc_color(screen[x][y][1]);
      position(x,y);
#ifdef MINGW
      printf("%c",screen[x][y][0]);
#else
      printf("\e[%d;%dH%c",y+1,x+1,screen[x][y][0]);
#endif
      }
    fflush(stdout);
    }
  }
#else
void restoreScreen() {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_copyArea(display,screenSave,xscreen,gc,0,0,screen_width,screen_height,0,0);
  rcs_closeGC(display,gc);
  rcs_invalidateViewport(viewport,0,0,screen_width,screen_height);
  }
#endif

/* Finds the first unused screenshotNNNN.<extension> in the working directory. */
static Int32 screenshot_name(char* filename,char* extension) {
  Int32 i;
  FILE* file;
  for (i=1; i<10000; i++) {
    sprintf(filename,"screenshot%04d.%s",i,extension);
    file = fopen(filename,"r");
    if (file == NULL) return 1;
    fclose(file);
    }
  return 0;
  }

#ifdef ASCII
void screenshot() {
  char  filename[32];
  FILE* file;
  Int32 x,y;
  Int32 last;
  if (!screenshot_name(filename,"txt")) return;
  file = fopen(filename,"w");
  if (file == NULL) return;

  for (y=0; y<25; y++) {
    last = 79;
    while (last >= 0 && screen[last][y][0] == ' ') last--;
    for (x=0; x<=last; x++) fputc(screen[x][y][0],file);
    fputc('\n',file);
    }

  fclose(file);
  }
#else
void screenshot() {
  char filename[32];
  if (!screenshot_name(filename,"bmp")) return;
  rcs_savePixmap(display,xscreen,screen_width,screen_height,filename);
  }
#endif

void draw(Int32 x1,Int32 y1,Int32 x2,Int32 y2,Int32 clr) {
#ifndef ASCII
  Int32 left,top;
  left = (x1 < x2) ? x1 : x2;
  top = (y1 < y2) ? y1 : y2;
  rcs_drawLine(display,xscreen,colors[clr],x1,y1,x2,y2);
  rcs_invalidateViewport(viewport,left,top,abs(x2 - x1) + 1,abs(y2 - y1) + 1);
#endif
  }

void graphmode() {
#ifdef ASCII
  clrscr();
#else
  clear_buffer();
#endif
  }

void textmode(Int32 mode) {
#ifdef ASCII
  clrscr();
#else
  clear_buffer();
#endif
  }

void textcolor(Int32 color) {
  text_color = color;
  }

void gotoxy(Int32 x,Int32 y) {
  cursor_x = x - 1;
  cursor_y = y - 1;
  }

void txt_write(char* buffer) {
#ifdef ASCII
  Int32 i;
  if (cursor_x < 0 || cursor_y < 0 || cursor_x > 79 || cursor_y > 24) return;
  position(cursor_x,cursor_y);
  gc_color(text_color);
  for (i=0; i<strlen(buffer); i++) {
    if (cursor_x >= 0 && cursor_x < 80 && cursor_y >= 0 && cursor_y < 25) {
      screen[cursor_x][cursor_y][0] = buffer[i];
      screen[cursor_x][cursor_y][1] = text_color;
      }
    cursor_x++;
    if (cursor_x == 80) {
      cursor_x = 0;
      cursor_y++;
      if (cursor_y > 24) cursor_y = 24;
      }
    }
  printf("%s",buffer);
  fflush(stdout);
#else
  Int32 i;
  Int32 x,y;
  char b2[2];
  rcs_GC gc;
  rcs_GC black;
  gc = rcs_openGC(display,window);
  black = rcs_openGC(display,window);
  rcs_namedForeground(display,gc,"black");
  gc_color(&gc,text_color);
  b2[1] = 0;
  for (i=0; i<strlen(buffer); i++) {
    x = cursor_x + text_offset_x;
    y = cursor_y + text_offset_y;
    b2[0] = buffer[i];
//    rcs_drawFilledBox(display,window,black,cursor_x*8,cursor_y*19,8,19);
//    rcs_drawFilledBox(display,xscreen,black,cursor_x*8,cursor_y*19,8,19);
//    rcs_drawString(display,window,gc,cursor_x * 8,10+cursor_y * 19,b2,0);
//    rcs_drawString(display,xscreen,gc,cursor_x * 8,10+cursor_y * 19,b2,0);
    rcs_drawFilledBox(display,xscreen,black,x*8,y*19,8,19);
    rcs_drawString(display,xscreen,gc,x * 8,10+y * 19,b2,0);
    rcs_invalidateViewport(viewport,x*8,y*19,8,19);
    invalidate_string(gc,x * 8,10+y * 19,b2);
    cursor_x++;
    if (cursor_x == 80) {
      cursor_x = 0;
      cursor_y++;
      if (cursor_y > 24) cursor_y = 24;
      }
    }
  rcs_closeGC(display,gc);
  rcs_closeGC(display,black);
#endif
  }

void txt_writeln(char* buffer) {
  txt_write(buffer);
  cursor_y++;
  cursor_x = 0;
  if (cursor_y > 24) cursor_y = 24;
  }

#ifndef ASCII
static Int32 is_modifier(Int32 key) {
  if (key == KEY_SHIFT_L || key == KEY_SHIFT_R) return 1;
  if (key == KEY_CTRL_L || key == KEY_CTRL_R) return 1;
  return 0;
  }
#endif

#ifndef ASCII
/* The window is closing or gone; without this the process lingers, music and all */
static void window_closed() {
  stop_music();
  exit(0);
  }
#endif

Int32 keypressed() {
  if (strlen(currentMacro) > 0) return 1;
#ifdef ASCII
#ifdef MINGW
  if (kbhit()) return 1;
  return 0;
#else
  Int32           c;
  fd_set         rfds;
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);
  c=select(FD_SETSIZE,&rfds,NULL,NULL,&tv);
  if (c!=0) return -1;
  return c;
#endif
#endif

#ifndef ASCII
  rcs_Event event;

  rcs_presentViewport(viewport);
  event = rcs_pollEvent(display);
  if (event.type == EVENT_CLOSE || event.type == EVENT_QUIT) window_closed();
  if (event.type == EVENT_KEY_PRESS && !is_modifier(event.d1)) {
    if (event.d1 == KY_SCREENSHOT) {
      screenshot();
      return 0;
      }

    key_event = event;
    return -1;
    }
  return 0;
#endif
  }

Int32 keytrans(Int32 n) {
#ifdef ASCII
  return n;
#else
  char          temp[32];
  switch (n) {
    case KEY_UP:return '8';
    case KEY_DOWN:return '2';
    case KEY_LEFT:return '4';
    case KEY_RIGHT:return '6';
#ifndef WIN32
    case KEY_KP_UP:return '8';
    case KEY_KP_DOWN:return '2';
    case KEY_KP_LEFT:return '4';
    case KEY_KP_RIGHT:return '6';
    case KEY_HOME:return '7';
    case KEY_KP_HOME:return '7';
    case KEY_END:return '1';
    case KEY_KP_END:return '1';
    case KEY_PAGE_UP:return '9';
    case KEY_KP_PAGE_UP:return '9';
    case KEY_PAGE_DN:return '3';
    case KEY_KP_PAGE_DN:return '3';
    case KEY_KP_BEGIN: return '5';
#endif
    case KEY_KP_1:return '1';
    case KEY_KP_2:return '2';
    case KEY_KP_3:return '3';
    case KEY_KP_4:return '4';
    case KEY_KP_5:return '5';
    case KEY_KP_6:return '6';
    case KEY_KP_7:return '7';
    case KEY_KP_8:return '8';
    case KEY_KP_9:return '9';
    case  KEY_F1: return KY_F1;
    case  KEY_F2: return KY_F2;
    case  KEY_F3: return KY_F3;
    case  KEY_F4: return KY_F4;
    case  KEY_F5: return KY_F5;
    case  KEY_F6: return KY_F6;
    case  KEY_F7: return KY_F7;
    case  KEY_F8: return KY_F8;
    case  KEY_F9: return KY_F9;
    case KEY_F10: return KY_F10;
#ifndef MINGW
    case KEY_F11: return KY_F11;
    case KEY_F12: return KY_F12;
#endif
    case  KEY_SHIFT_F1: return KY_S_F1;
    case  KEY_SHIFT_F2: return KY_S_F2;
    case  KEY_SHIFT_F3: return KY_S_F3;
    case  KEY_SHIFT_F4: return KY_S_F4;
    case  KEY_SHIFT_F5: return KY_S_F5;
    case  KEY_SHIFT_F6: return KY_S_F6;
    case  KEY_SHIFT_F7: return KY_S_F7;
    case  KEY_SHIFT_F8: return KY_S_F8;
    case  KEY_SHIFT_F9: return KY_S_F9;
    case KEY_SHIFT_F10: return KY_S_F10;
#ifndef MINGW
    case KEY_SHIFT_F11: return KY_S_F11;
    case KEY_SHIFT_F12: return KY_S_F12;
#endif
    case  KEY_ALT_F1: return KY_A_F1;
    case  KEY_ALT_F2: return KY_A_F2;
    case  KEY_ALT_F3: return KY_A_F3;
    case  KEY_ALT_F4: return KY_A_F4;
    case  KEY_ALT_F5: return KY_A_F5;
    case  KEY_ALT_F6: return KY_A_F6;
    case  KEY_ALT_F7: return KY_A_F7;
    case  KEY_ALT_F8: return KY_A_F8;
    case  KEY_ALT_F9: return KY_A_F9;
    case KEY_ALT_F10: return KY_A_F10;
#ifndef MINGW
    case KEY_ALT_F11: return KY_A_F11;
    case KEY_ALT_F12: return KY_A_F12;
#endif
    case  KEY_CTRL_F1: return KY_C_F1;
    case  KEY_CTRL_F2: return KY_C_F2;
    case  KEY_CTRL_F3: return KY_C_F3;
    case  KEY_CTRL_F4: return KY_C_F4;
    case  KEY_CTRL_F5: return KY_C_F5;
    case  KEY_CTRL_F6: return KY_C_F6;
    case  KEY_CTRL_F7: return KY_C_F7;
    case  KEY_CTRL_F8: return KY_C_F8;
    case  KEY_CTRL_F9: return KY_C_F9;
    case KEY_CTRL_F10: return KY_C_F10;
#ifndef MINGW
    case KEY_CTRL_F11: return KY_C_F11;
    case KEY_CTRL_F12: return KY_C_F12;
#endif
    }
      if (show_keys == 'Y') {
        sprintf(temp,"key=%d",n);
        msg(temp,15);
        }
  return n;
#endif
  }

static Int32 next_key() {
#ifdef ASCII
#ifdef MINGW
  Int32           c;
  char          temp[32];
  unsigned char          key;
  c = 0;
  key = getch();
  c = key;
  if (key == 224 || key == 0) {
      key = getch();
      if (key == 72) c = '8';
      if (key == 80) c = '2';
      if (key == 75) c = '4';
      if (key == 77) c = '6';
      if (key == 71) c = '7';
      if (key == 73) c = '9';
      if (key == 79) c = '1';
      if (key == 81) c = '3';
      if (key == 59) c = KY_F1;     // F1
      if (key == 60) c = KY_F2;     // F2
      if (key == 61) c = KY_F3;     // F3
      if (key == 62) c = KY_F4;     // F4
      if (key == 63) c = KY_F5;     // F5
      if (key == 64) c = KY_F6;     // F6
      if (key == 65) c = KY_F7;     // F7
      if (key == 66) c = KY_F8;     // F8
      if (key == 67) c = KY_F9;     // F9
      if (key == 68) c = KY_F10;     // F10
      if (key == 84) c = KY_S_F1;     // Shift F1
      if (key == 85) c = KY_S_F2;     // Shift F2
      if (key == 86) c = KY_S_F3;     // Shift F3
      if (key == 87) c = KY_S_F4;     // Shift F4
      if (key == 88) c = KY_S_F5;     // Shift F5
      if (key == 89) c = KY_S_F6;     // Shift F6
      if (key == 90) c = KY_S_F7;     // Shift F7
      if (key == 91) c = KY_S_F8;     // Shift F8
      if (key == 92) c = KY_S_F9;     // Shift F9
      if (key == 93) c = KY_S_F10;     // Shift F10
      if (key == 94) c = KY_C_F1;     // Ctrl F1
      if (key == 95) c = KY_C_F2;     // Ctrl F2
      if (key == 96) c = KY_C_F3;     // Ctrl F3
      if (key == 97) c = KY_C_F4;     // Ctrl F4
      if (key == 98) c = KY_C_F5;     // Ctrl F5
      if (key == 99) c = KY_C_F6;     // Ctrl F6
      if (key == 100) c = KY_C_F7;     // Ctrl F7
      if (key == 101) c = KY_C_F8;     // Ctrl F8
      if (key == 102) c = KY_C_F9;     // Ctrl F9
      if (key == 103) c = KY_C_F10;     // Ctrl F10
      if (key == 104) c = KY_A_F1;     // Alt F1
      if (key == 105) c = KY_A_F2;     // Alt F2
      if (key == 106) c = KY_A_F3;     // Alt F3
      if (key == 107) c = KY_A_F4;     // Alt F4
      if (key == 108) c = KY_A_F5;     // Alt F5
      if (key == 109) c = KY_A_F6;     // Alt F6
      if (key == 110) c = KY_A_F7;     // Alt F7
      if (key == 111) c = KY_A_F8;     // Alt F8
      if (key == 112) c = KY_A_F9;     // Alt F9
      if (key == 113) c = KY_A_F10;     // Alt F10
      if (show_keys == 'Y') {
        sprintf(temp,"key=%d",key);
        msg(temp,15);
        }
      }
  if (c == 10) c = 13;
  return c;
#else                 // ifdef MINGW
  char temp[64];
  Int32           c;
  char          key;
  c = 0;
  while (c == 0) {
/*    c=fgetc(stdin); */
    read(0,&key,1);
    c = key;
    if (esc_mode > 0) {
      esc_seq[esc_mode-1] = c;
      esc_seq[esc_mode] = 0;
      esc_mode++;
      if (c>='A' && c<='Z' && c != 'O') esc_mode = 0;
      if (c>='a' && c<='~') esc_mode = 0;
      if (esc_mode > 10) esc_mode = 0;
      c = 0;
      if (esc_mode == 0) { 
        if (strcmp(esc_seq,"[A") == 0) c = '8';
        if (strcmp(esc_seq,"[B") == 0) c = '2';
        if (strcmp(esc_seq,"[C") == 0) c = '6';
        if (strcmp(esc_seq,"[D") == 0) c = '4';
        if (strcmp(esc_seq,"[E") == 0) c = '5';
        if (strcmp(esc_seq,"[H") == 0) c = '7';
        if (strcmp(esc_seq,"[F") == 0) c = '1';
        if (strcmp(esc_seq,"[4~") == 0) c = '1';
        if (strcmp(esc_seq,"[1~") == 0) c = '7';
        if (strcmp(esc_seq,"[5~") == 0) c = '9';
        if (strcmp(esc_seq,"[6~") == 0) c = '3';
        if (strcmp(esc_seq,"OP") == 0) c = KY_F1;       /* F1 */
        if (strcmp(esc_seq,"OQ") == 0) c = KY_F2;       /* F2 */
        if (strcmp(esc_seq,"OR") == 0) c = KY_F3;       /* F3 */
        if (strcmp(esc_seq,"OS") == 0) c = KY_F4;       /* F4 */
        if (strcmp(esc_seq,"[15~") == 0) c = KY_F5;     /* F5 */
        if (strcmp(esc_seq,"[17~") == 0) c = KY_F6;     /* F6 */
        if (strcmp(esc_seq,"[18~") == 0) c = KY_F7;     /* F7 */
        if (strcmp(esc_seq,"[19~") == 0) c = KY_F8;     /* F8 */
        if (strcmp(esc_seq,"[20~") == 0) c = KY_F9;     /* F9 */
        if (strcmp(esc_seq,"[21~") == 0) c = KY_F10;     /* F10 */
        if (strcmp(esc_seq,"[1;2P") == 0) c = KY_S_F1;    /* Shift F1 */
        if (strcmp(esc_seq,"[1;2Q") == 0) c = KY_S_F2;    /* Shift F2 */
        if (strcmp(esc_seq,"[1;2R") == 0) c = KY_S_F3;    /* Shift F3 */
        if (strcmp(esc_seq,"[1;2S") == 0) c = KY_S_F4;    /* Shift F4 */
        if (strcmp(esc_seq,"[15;2~") == 0) c = KY_S_F5;   /* Shift F5 */
        if (strcmp(esc_seq,"[17;2~") == 0) c = KY_S_F6;   /* Shift F6 */
        if (strcmp(esc_seq,"[18;2~") == 0) c = KY_S_F7;   /* Shift F7 */
        if (strcmp(esc_seq,"[19;2~") == 0) c = KY_S_F8;   /* Shift F8 */
        if (strcmp(esc_seq,"[20;2~") == 0) c = KY_S_F9;   /* Shift F9 */
        if (strcmp(esc_seq,"[21;2~") == 0) c = KY_S_F10;   /* Shift F10 */
        if (strcmp(esc_seq,"[1;5P") == 0) c = KY_C_F1;    /* Ctrl F1 */
        if (strcmp(esc_seq,"[1;5Q") == 0) c = KY_C_F2;    /* Ctrl F2 */
        if (strcmp(esc_seq,"[1;5R") == 0) c = KY_C_F3;    /* Ctrl F3 */
        if (strcmp(esc_seq,"[1;5S") == 0) c = KY_C_F4;    /* Ctrl F4 */
        if (strcmp(esc_seq,"[15;5~") == 0) c = KY_C_F5;    /* Ctrl F5 */
        if (strcmp(esc_seq,"[17;5~") == 0) c = KY_C_F6;    /* Ctrl F6 */
        if (strcmp(esc_seq,"[18;5~") == 0) c = KY_C_F7;    /* Ctrl F7 */
        if (strcmp(esc_seq,"[19;5~") == 0) c = KY_C_F8;    /* Ctrl F8 */
        if (strcmp(esc_seq,"[20;5~") == 0) c = KY_C_F9;    /* Ctrl F9 */
        if (strcmp(esc_seq,"[21;5~") == 0) c = KY_C_F10;    /* Ctrl F10 */
        if (strcmp(esc_seq,"[1;3P") == 0) c = KY_A_F1;    /* Alt F1 */
        if (strcmp(esc_seq,"[1;3Q") == 0) c = KY_A_F2;    /* Alt F2 */
        if (strcmp(esc_seq,"[1;3R") == 0) c = KY_A_F3;    /* Alt F3 */
        if (strcmp(esc_seq,"[1;3S") == 0) c = KY_A_F4;    /* Alt F4 */
        if (strcmp(esc_seq,"[15;3~") == 0) c = KY_A_F5;    /* Alt F5 */
        if (strcmp(esc_seq,"[17;3~") == 0) c = KY_A_F6;    /* Alt F6 */
        if (strcmp(esc_seq,"[18;3~") == 0) c = KY_A_F7;    /* Alt F7 */
        if (strcmp(esc_seq,"[19;3~") == 0) c = KY_A_F8;    /* Alt F8 */
        if (strcmp(esc_seq,"[20;3~") == 0) c = KY_A_F9;    /* Alt F9 */
        if (strcmp(esc_seq,"[21;3~") == 0) c = KY_A_F10;    /* Alt F10 */
        if (show_keys == 'Y') {
          sprintf(temp,"seq=%s",esc_seq);
          msg(temp,15);
          }
        }
      }
    else if (c == 27 && keypressed()) {
      esc_mode = 1;
      esc_seq[0] = 0;
      c = 0;
      }
    }
  if (c == 10) c = 13;
  return c;
#endif
#endif

#ifndef ASCII
  rcs_Event event;
  char      flag;
  flag = ' ';
  rcs_presentViewport(viewport);
  if (key_event.type == EVENT_KEY_PRESS) {
    key_event.type = EVENT_NONE;
    return keytrans(key_event.d1);
    }
  while (flag == ' ') {
    event = rcs_getEvent(display);
    if (event.type == EVENT_CLOSE || event.type == EVENT_QUIT) window_closed();
    if (event.type == EVENT_KEY_PRESS) {
      if (!is_modifier(event.d1))
        return keytrans(event.d1);
      }
    }
  return 0;
#endif
  }

Int32 readkey() {
  Int32 key;
  key = next_key();
  while (key == KY_SCREENSHOT) {
    screenshot();
    key = next_key();
    }
  return key;
  }


/*
void putpixel(Int32 x,Int32 y,Int32 clr) {
#ifndef ASCII
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_openGC(display,window);
  gc_color(&gc,clr);
  rcs_drawPoint(display,window,gc,x,y);
  rcs_drawPoint(display,xscreen,gc,x,y);
  rcs_closeGC(display,gc);
#endif
  }
*/

void flush() {
#ifndef ASCII
  rcs_presentViewport(viewport);
#endif
  }

void open_display() {
#ifndef ASCII
  gc = rcs_openGC(display,window);
#endif
  }

void close_display() {
#ifndef ASCII
  rcs_presentViewport(viewport);
  rcs_closeGC(display,gc);
#endif
  }

void plot(Int32 x,Int32 y,Int32 clr) {
#ifndef ASCII
//  gc_color(&gc,clr);
  rcs_drawPoint(display,xscreen,colors[clr],x,y);
  rcs_invalidateViewport(viewport,x,y,1,1);
#endif
  }

void setfillstyle(Int32 st,Int32 clr) {
  g_color = clr;
  }

void bar(Int32 x1,Int32 y1,Int32 x2,Int32 y2) {
#ifndef ASCII
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawFilledBox(display,xscreen,gc,x1,y1,x2-x1,y2-y1);
  rcs_closeGC(display,gc);
  rcs_invalidateViewport(viewport,x1,y1,x2-x1,y2-y1);
#endif
  }

void setcolor(Int32 clr) {
  text_color = clr;
  g_color = clr;
  }

void settextstyle(Int32 a,Int32 b,Int32 c) {
  }

void outtext_xy(Int32 x,Int32 y,char* buffer) {
#ifdef ASCII
  Int32 oldX,oldY;
  x += 1;
  y += 1;
  oldX = cursor_x;
  oldY = cursor_y;
  gotoxy(x,y);
  txt_write(buffer);
  cursor_x = oldX;
  cursor_y = oldY;
#else
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x*8,y*11,buffer,0);
  invalidate_string(gc,x*8,y*11,buffer);
  rcs_closeGC(display,gc);
#endif
  }

void outtextxy(Int32 x,Int32 y,char* buffer) {
#ifdef ASCII
  Int32 oldX,oldY;
  oldX = cursor_x;
  oldY = cursor_y;
  gotoxy(x/8+1,y/10+1);
  txt_write(buffer);
  cursor_x = oldX;
  cursor_y = oldY;
#else
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x,y,buffer,0);
  invalidate_string(gc,x,y,buffer);
  rcs_closeGC(display,gc);
#endif
  }

void outtextxyclr(Int32 x,Int32 y,char* buffer) {
#ifdef ASCII
  Int32 oldX,oldY;
  oldX = cursor_x;
  oldY = cursor_y;
  gotoxy(x/8+1,y/10+1);
  txt_write(buffer);
  cursor_x = oldX;
  cursor_y = oldY;
#else
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x,y,buffer,-1);
  invalidate_string(gc,x,y,buffer);
  rcs_closeGC(display,gc);
#endif
  }

void putimage(Int32 x,Int32 y,char** img,Int32 typ) {
#ifdef ASCII
  Int32 oldX,oldY,oldClr;
  char buffer[10];
  oldX = cursor_x;
  oldY = cursor_y;
  oldClr = text_color;
  x -= 10;
  y -= 10;
  gotoxy(x/25,y/27);
  buffer[0] = img[0][0];
  buffer[1] = 0;
  if (img[0][1] >='A' && img[0][1] <='P') text_color = img[0][1] - 'A';
    else text_color = 7;
  txt_write(buffer);
  if (img[0][1] != 0 && img[0][2] != 0) {
    if (img[0][3] >='A' && img[0][3] <='P') text_color = img[0][3] - 'A';
      else text_color = 7;
    buffer[0] = img[0][2];
    gotoxy(x/25+1,y/27);
    txt_write(buffer);
    }
  cursor_x = oldX;
  cursor_y = oldY;
  text_color = oldClr;
#else
  rcs_GC gc;
  Int32  width,height;
  gc = rcs_openGC(display,window);
  rcs_drawPixmap(display,xscreen,gc,x,y,(const char**)img);
  rcs_closeGC(display,gc);
  sscanf(img[0],"%d %d",&width,&height);
  rcs_invalidateViewport(viewport,x,y,width,height);
#endif
  }

void delay(Int32 tm) {
#ifndef ASCII
  rcs_presentViewport(viewport);
#endif
  usleep(tm * 1000);
/*
  Int32 sec;
  long te;
  long ts;
  struct timeval tim;
  sec = tm / 1000;
  tm -= (sec * 1000);
  gettimeofday(&tim,NULL);
  te = ((tim.tv_sec & 0xff) * 1000000) + tim.tv_usec;
  te += (tm * 1000);
  gettimeofday(&tim,NULL);
  ts = ((tim.tv_sec & 0xff) * 1000000) + tim.tv_usec;
  while (ts < te && abs(ts-te) <= (tm*1000)) {
    gettimeofday(&tim,NULL);
    ts = ((tim.tv_sec & 0xff) * 1000000) + tim.tv_usec;
    }
*/
  }

/*
void delay(Int32 tm) {
  Int32 sec;
  struct timeval ts,te;
debugLog("entering delay\n");
  sec = tm / 1000;
  tm -= (sec * 1000);
  gettimeofday(&te,NULL);
  te.tv_sec += sec;
  te.tv_usec += (tm * 1000);
debugLog("Before usec to seconds\n");
  while (te.tv_usec >= 1000000) {
    te.tv_sec++;
    te.tv_usec -= 1000000;
    }
debugLog("After usec to seconds\n");
  gettimeofday(&ts,NULL);
  while (ts.tv_sec < te.tv_sec || ts.tv_usec < te.tv_usec) {
sprintf(debugMSG,"%ld %ld \n",ts.tv_sec-te.tv_sec,ts.tv_usec-te.tv_usec);
debugLog(debugMSG);
    gettimeofday(&ts,NULL);
    }
debugLog("exiting delay\n");
  }
*/
Int32 p_read(char* buffer) {
  Int32 temp_x,temp_y;
  unsigned char key;
/*  strcpy(buffer,""); */
  key = 0;
  temp_x = cursor_x;
  temp_y = cursor_y;
  gotoxy(temp_x+1,temp_y+1);
  txt_write(buffer);
  txt_write("_");
  while (key != 13 && key != 27) {
    key = inkey();
    if (key == KY_F1) strcpy(currentMacro, macros[0]);
    if (key == KY_F2) strcpy(currentMacro, macros[1]);
    if (key == KY_F3) strcpy(currentMacro, macros[2]);
    if (key == KY_F4) strcpy(currentMacro, macros[3]);
    if (key == KY_F5) strcpy(currentMacro, macros[4]);
    if (key == KY_F6) strcpy(currentMacro, macros[5]);
    if (key == KY_F7) strcpy(currentMacro, macros[6]);
    if (key == KY_F8) strcpy(currentMacro, macros[7]);
    if (key == KY_F9) strcpy(currentMacro, macros[8]);
    if (key == KY_F10) strcpy(currentMacro, macros[9]);
    if (key == KY_F11) strcpy(currentMacro, macros[10]);
    if (key == KY_F12) strcpy(currentMacro, macros[11]);
    if (key > 31 && key < 127) {
      buffer[strlen(buffer) + 1] = 0;
      buffer[strlen(buffer)] = key;
      gotoxy(temp_x+1,temp_y+1);
      txt_write(buffer);
      txt_write("_");
      }
    if ((key == 8 || key == 127) && strlen(buffer) > 0) {
      buffer[strlen(buffer) - 1] = 0;
      gotoxy(temp_x+1,temp_y+1);
      txt_write(buffer);
      txt_write("_ ");
      }
    }
  gotoxy(temp_x+1,temp_y+1);
  txt_write(buffer);
  txt_write("  ");
  cursor_x = temp_x;
  cursor_y = temp_y;
  if (key == 27) return -1;
  return 0;
  }

void readln(char* buffer) {
  p_read(buffer);
  /*
  cursor_y++;
  cursor_x = 0;
  if (cursor_y > 24) cursor_y = 24;
  */
  }

Int32 restoreTerm() {
#ifdef ASCII
#ifdef MINGW
  show_cursor();
#endif
#ifndef MINGW
/*  terminal.c_lflag |= ICANON; */
  if (tcsetattr(0,TCSANOW,&original)!=0) return -1;
  printf("\e[0;49m");
  printf("\E[?25h");
  printf("\E[H\E[2J");
#endif
#endif
  return 0;
  }


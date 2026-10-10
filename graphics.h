#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#define LINK extern

/* ************************************* */
/* ***** Definitions of MS-Windows ***** */
/* ************************************* */

#include <windows.h>

typedef HWND rcs_Window;
typedef HWND rcs_Pixmap;

#define KEY_F1        (VK_F1+0xf000)
#define KEY_F2        (VK_F2+0xf000)
#define KEY_F3        (VK_F3+0xf000)
#define KEY_F4        (VK_F4+0xf000)
#define KEY_F5        (VK_F5+0xf000)
#define KEY_F6        (VK_F6+0xf000)
#define KEY_F7        (VK_F7+0xf000)
#define KEY_F8        (VK_F8+0xf000)
#define KEY_F9        (VK_F9+0xf000)
#define KEY_F10       (VK_F10+0xf000)
#define KEY_F11       (VK_F11+0xf000)
#define KEY_F12       (VK_F12+0xf000)
#define KEY_UP        (VK_UP+0xf000)
#define KEY_DOWN      (VK_DOWN+0xf000)
#define KEY_LEFT      (VK_LEFT+0xf000)
#define KEY_RIGHT     (VK_RIGHT+0xf000)
#define KEY_HOME      (VK_HOME+0xf000)
#define KEY_END       (VK_END+0xf000)
#define KEY_PAGE_UP   (VK_PRIOR+0xf000)
#define KEY_PAGE_DN   (VK_NEXT+0xf000)
#define KEY_INSERT    (VK_INSERT+0xf000)
#define KEY_DELETE    (VK_DELETE+0xf000)
#define KEY_KP_S5     (VK_CLEAR+0xf000)
#define KEY_KP_MUL    (VK_MULTIPLY+0xf000)
#define KEY_KP_DIV    (VK_DIVIDE+0xf000)
#define KEY_KP_ADD    (VK_ADD+0xf000)
#define KEY_KP_SUB    (VK_SUBTRACT+0xf000)
#define KEY_SHIFT_R   (VK_SHIFT+0xf000)
#define KEY_CTRL_L    (VK_CONTROL+0xf000)
#define KEY_CTRL_R    (VK_CONTROL+0xf000)
#define KEY_KP_0      (VK_NUMPAD0+0xf000)
#define KEY_KP_1      (VK_NUMPAD1+0xf000)
#define KEY_KP_2      (VK_NUMPAD2+0xf000)
#define KEY_KP_3      (VK_NUMPAD3+0xf000)
#define KEY_KP_4      (VK_NUMPAD4+0xf000)
#define KEY_KP_5      (VK_NUMPAD5+0xf000)
#define KEY_KP_6      (VK_NUMPAD6+0xf000)
#define KEY_KP_7      (VK_NUMPAD7+0xf000)
#define KEY_KP_8      (VK_NUMPAD8+0xf000)
#define KEY_KP_9      (VK_NUMPAD9+0xf000)
#define KEY_KP_UP     KEY_KP_8
#define KEY_KP_DOWN   KEY_KP_2
#define KEY_KP_LEFT   KEY_KP_4
#define KEY_KP_RIGHT  KEY_KP_6
#define KEY_KP_HOME   KEY_KP_7
#define KEY_KP_END    KEY_KP_1
#define KEY_KP_PAGE_UP  KEY_KP_9
#define KEY_KP_PAGE_DN  KEY_KP_3
#define KEY_KP_BEGIN    KEY_KP_5
#define KEY_TAB       9
#define KEY_BACK_SP   8
#define KEY_SHIFT_F1  (KEY_F1 + 0x10000)
#define KEY_SHIFT_F2  (KEY_F2 + 0x10000)
#define KEY_SHIFT_F3  (KEY_F3 + 0x10000)
#define KEY_SHIFT_F4  (KEY_F4 + 0x10000)
#define KEY_SHIFT_F5  (KEY_F5 + 0x10000)
#define KEY_SHIFT_F6  (KEY_F6 + 0x10000)
#define KEY_SHIFT_F7  (KEY_F7 + 0x10000)
#define KEY_SHIFT_F8  (KEY_F8 + 0x10000)
#define KEY_SHIFT_F9  (KEY_F9 + 0x10000)
#define KEY_SHIFT_F10 (KEY_F10 + 0x10000)
#define KEY_ALT_F1    (KEY_F1 + 0x20000)
#define KEY_ALT_F2    (KEY_F2 + 0x20000)
#define KEY_ALT_F3    (KEY_F3 + 0x20000)
#define KEY_ALT_F4    (KEY_F4 + 0x20000)
#define KEY_ALT_F5    (KEY_F5 + 0x20000)
#define KEY_ALT_F6    (KEY_F6 + 0x20000)
#define KEY_ALT_F7    (KEY_F7 + 0x20000)
#define KEY_ALT_F8    (KEY_F8 + 0x20000)
#define KEY_ALT_F9    (KEY_F9 + 0x20000)
#define KEY_ALT_F10   (KEY_F10 + 0x20000)
#define KEY_CTRL_F1   (KEY_F1 + 0x40000)
#define KEY_CTRL_F2   (KEY_F2 + 0x40000)
#define KEY_CTRL_F3   (KEY_F3 + 0x40000)
#define KEY_CTRL_F4   (KEY_F4 + 0x40000)
#define KEY_CTRL_F5   (KEY_F5 + 0x40000)
#define KEY_CTRL_F6   (KEY_F6 + 0x40000)
#define KEY_CTRL_F7   (KEY_F7 + 0x40000)
#define KEY_CTRL_F8   (KEY_F8 + 0x40000)
#define KEY_CTRL_F9   (KEY_F9 + 0x40000)
#define KEY_CTRL_F10  (KEY_F10 + 0x40000)
#define KEY_SHIFT_L   (VK_SHIFT+0xf000)
#define KEY_SHIFT_R   (VK_SHIFT+0xf000)
#define KEY_CTRL_M    ('M' + 0xf000 + 0x40000)

typedef struct {
  HINSTANCE    hInstance;
  TCHAR        className[80];
  rcs_Window*  pixmapIDs;
  int          numPixmaps;
  HFONT        font;
  int          fontAscent;
  int          fontHeight;
  int          fontOverhang;
  } rcs_Display;

typedef struct _GC {
  COLORREF foreground;
  COLORREF background;
  HFONT    font;
  } *rcs_GC;


/* ****************************** */
/* ***** Shared definitions ***** */
/* ****************************** */
#define EVENT_NONE           0
#define EVENT_MOTION         1
#define EVENT_BUTTON_PRESS   2
#define EVENT_BUTTON_RELEASE 3
#define EVENT_EXPOSE         4
#define EVENT_KEY_PRESS      5
#define EVENT_KEY_RELEASE    6
#define EVENT_CONFIGURE      7
#define EVENT_LEAVE          8
#define EVENT_ENTER          9
#define EVENT_FOCUSIN        10
#define EVENT_FOCUSOUT       11
#define EVENT_CLOSE          98
#define EVENT_QUIT           99

typedef struct {
  int          type;
  rcs_Window   window;
  int          d1,d2,d3,d4,d5;
  } rcs_Event;

/* Shows a fixed-size pixmap scaled to fill a window at its own aspect ratio, centred on black */
typedef struct _rcs_Viewport* rcs_Viewport;

/* ******************************* */
/* ***** Function prototypes ***** */
/* ******************************* */
LINK void         rcs_closeGC(rcs_Display* d,rcs_GC gc);
LINK void         rcs_copyArea(rcs_Display*,rcs_Window,rcs_Window,rcs_GC,int sx,int sy,int w,int h,int dx,int dy);
LINK rcs_Pixmap   rcs_createPixmap(rcs_Display*,rcs_Window,int,int);
LINK rcs_Viewport rcs_createViewport(rcs_Display* d,rcs_Window win,rcs_Pixmap buffer,int width,int height);
LINK rcs_Window   rcs_createWindow(rcs_Display* d,rcs_Window parent, int x,int y,int width,int height);
LINK void         rcs_drawFilledBox(rcs_Display*,rcs_Window win,rcs_GC gc,int x,int y,int w,int h);
LINK void         rcs_drawLine(rcs_Display*,rcs_Window win,rcs_GC gc,int x1,int y1,int x2,int y2);
LINK void         rcs_drawPixmap(rcs_Display* d,rcs_Window win,rcs_GC gc,int dx,int dy,const char** data);
LINK void         rcs_drawPoint(rcs_Display*,rcs_Window win,rcs_GC gc,int x,int y);
LINK void         rcs_drawString(rcs_Display* d,rcs_Window win,rcs_GC gc,int x,int y,char* buffer,int clr);
LINK rcs_Event    rcs_getEvent(rcs_Display*);
LINK void         rcs_invalidateViewport(rcs_Viewport v,int x,int y,int w,int h);
LINK void         rcs_namedBackground(rcs_Display* d,rcs_GC gc,const char* color);
LINK void         rcs_namedForeground(rcs_Display* d,rcs_GC gc,const char* color);
LINK rcs_Display *rcs_openDisplay(char* display_name);
LINK rcs_GC       rcs_openGC(rcs_Display* d,rcs_Window win);
LINK rcs_Event    rcs_pollEvent(rcs_Display*);
LINK void         rcs_presentViewport(rcs_Viewport v);
LINK rcs_Window   rcs_rootWindow(rcs_Display* d);
LINK int          rcs_savePixmap(rcs_Display* d,rcs_Pixmap pixmap,int width,int height,char* filename);
LINK void         rcs_setWindowName(rcs_Display*,rcs_Window,char*);
LINK void         rcs_showWindow(rcs_Display* d,rcs_Window win);
LINK void         rcs_stringBounds(rcs_Display* d,rcs_GC gc,int x,int y,char* buffer,int* left,int* top,int* width,int* height);

#endif


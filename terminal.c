/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "graphics.h"

/* Drawing goes to xscreen only; the viewport shows it in the window when the game flushes or waits */
rcs_Event          key_event;
rcs_Window         window;
rcs_Display*       display;
rcs_Pixmap         xscreen;
rcs_Pixmap         screenSave;
rcs_Viewport       viewport;
rcs_GC             colors[16];
rcs_GC             gc;

#include "header.h"
#include "terminal.h"

Int32 cursor_x;
Int32 cursor_y;
Int32 text_color;
Int32 g_color;

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

void clrscr() {
  clear_buffer();
  }

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

void init_graph() {
  Int32 i;
  display = rcs_openDisplay(":0.0");
  window =rcs_createWindow(display,rcs_rootWindow(display),0,0,screen_width,screen_height);
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

void saveScreen() {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_copyArea(display,xscreen,screenSave,gc,0,0,screen_width,screen_height,0,0);
  rcs_closeGC(display,gc);
  }

void restoreScreen() {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  rcs_copyArea(display,screenSave,xscreen,gc,0,0,screen_width,screen_height,0,0);
  rcs_closeGC(display,gc);
  rcs_invalidateViewport(viewport,0,0,screen_width,screen_height);
  }

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

void screenshot() {
  char filename[32];
  if (!screenshot_name(filename,"bmp")) return;
  rcs_savePixmap(display,xscreen,screen_width,screen_height,filename);
  }

void draw(Int32 x1,Int32 y1,Int32 x2,Int32 y2,Int32 clr) {
  Int32 left,top;
  left = (x1 < x2) ? x1 : x2;
  top = (y1 < y2) ? y1 : y2;
  rcs_drawLine(display,xscreen,colors[clr],x1,y1,x2,y2);
  rcs_invalidateViewport(viewport,left,top,abs(x2 - x1) + 1,abs(y2 - y1) + 1);
  }

void graphmode() {
  clear_buffer();
  }

void textmode(Int32 mode) {
  clear_buffer();
  }

void textcolor(Int32 color) {
  text_color = color;
  }

void gotoxy(Int32 x,Int32 y) {
  cursor_x = x - 1;
  cursor_y = y - 1;
  }

void txt_write(char* buffer) {
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
  }

void txt_writeln(char* buffer) {
  txt_write(buffer);
  cursor_y++;
  cursor_x = 0;
  if (cursor_y > 24) cursor_y = 24;
  }

static Int32 is_modifier(Int32 key) {
  if (key == KEY_SHIFT_L || key == KEY_SHIFT_R) return 1;
  if (key == KEY_CTRL_L || key == KEY_CTRL_R) return 1;
  return 0;
  }

/* The window is closing or gone; without this the process lingers, music and all */
static void window_closed() {
  stop_music();
  exit(0);
  }

Int32 keypressed() {
  rcs_Event event;
  if (strlen(currentMacro) > 0) return 1;

  rcs_presentViewport(viewport);
  event = rcs_pollEvent(display);
  if (event.type == EVENT_CLOSE || event.type == EVENT_QUIT) window_closed();
  if (event.type == EVENT_KEY_PRESS && !is_modifier(event.d1)) {
    if (event.d1 == KY_SCREENSHOT) {
      screenshot();
      return 0;
      }

    if (event.d1 == KEY_CTRL_M) {
      toggle_music();
      return 0;
      }

    key_event = event;
    return -1;
    }
  return 0;
  }

Int32 keytrans(Int32 n) {
  char          temp[32];
  switch (n) {
    case KEY_UP:return '8';
    case KEY_DOWN:return '2';
    case KEY_LEFT:return '4';
    case KEY_RIGHT:return '6';
    case KEY_CTRL_M:return KY_MUSIC;
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
    }
      if (show_keys == 'Y') {
        sprintf(temp,"key=%d",n);
        msg(temp,15);
        }
  return n;
  }

static Int32 next_key() {
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
  }

/* Ctrl+P and Ctrl+M work on every screen, so the game never sees them */
Int32 readkey() {
  Int32 key;
  key = next_key();
  while (key == KY_SCREENSHOT || key == KY_MUSIC) {
    if (key == KY_SCREENSHOT) screenshot();
    else toggle_music();
    key = next_key();
    }
  return key;
  }

void flush() {
  rcs_presentViewport(viewport);
  }

void open_display() {
  gc = rcs_openGC(display,window);
  }

void close_display() {
  rcs_presentViewport(viewport);
  rcs_closeGC(display,gc);
  }

void plot(Int32 x,Int32 y,Int32 clr) {
//  gc_color(&gc,clr);
  rcs_drawPoint(display,xscreen,colors[clr],x,y);
  rcs_invalidateViewport(viewport,x,y,1,1);
  }

void setfillstyle(Int32 st,Int32 clr) {
  g_color = clr;
  }

void bar(Int32 x1,Int32 y1,Int32 x2,Int32 y2) {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawFilledBox(display,xscreen,gc,x1,y1,x2-x1,y2-y1);
  rcs_closeGC(display,gc);
  rcs_invalidateViewport(viewport,x1,y1,x2-x1,y2-y1);
  }

void setcolor(Int32 clr) {
  text_color = clr;
  g_color = clr;
  }

void settextstyle(Int32 a,Int32 b,Int32 c) {
  }

void outtext_xy(Int32 x,Int32 y,char* buffer) {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x*8,y*11,buffer,0);
  invalidate_string(gc,x*8,y*11,buffer);
  rcs_closeGC(display,gc);
  }

void outtextxy(Int32 x,Int32 y,char* buffer) {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x,y,buffer,0);
  invalidate_string(gc,x,y,buffer);
  rcs_closeGC(display,gc);
  }

void outtextxyclr(Int32 x,Int32 y,char* buffer) {
  rcs_GC gc;
  gc = rcs_openGC(display,window);
  gc_color(&gc,g_color);
  rcs_drawString(display,xscreen,gc,x,y,buffer,-1);
  invalidate_string(gc,x,y,buffer);
  rcs_closeGC(display,gc);
  }

void putimage(Int32 x,Int32 y,char** img,Int32 typ) {
  rcs_GC gc;
  Int32  width,height;
  gc = rcs_openGC(display,window);
  rcs_drawPixmap(display,xscreen,gc,x,y,(const char**)img);
  rcs_closeGC(display,gc);
  sscanf(img[0],"%d %d",&width,&height);
  rcs_invalidateViewport(viewport,x,y,width,height);
  }

void delay(Int32 tm) {
  rcs_presentViewport(viewport);
  Sleep(tm);
  }

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


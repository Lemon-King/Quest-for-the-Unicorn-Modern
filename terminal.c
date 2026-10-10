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
#include "header.h"
#include "terminal.h"
#include "platform.h"

/* The window, and the viewport that shows the canvas in it */
static rcs_Display*      display;
static rcs_Window        window;
static rcs_Viewport      viewport;
static rcs_Event         key_event;

/* Drawing goes to the canvas only; present() shows what changed when the game flushes or waits.
   saved is the copy saveScreen() keeps; sprites and font draw on either. */
static CANVAS*           screen;
static CANVAS*           saved;
static SPRITE_CACHE*     sprites;
static const GLYPH_FONT* font;

/* CLR_BLACK..CLR_WHITE as the GDI build resolved their X11 names */
static const UInt32 PALETTE[16] = {
  0x000000,   /* black      */
  0x0000cd,   /* blue3      */
  0x00cd00,   /* green3     */
  0x00ffff,   /* cyan       */
  0xff0000,   /* red        */
  0xa020f0,   /* purple     */
  0xa52a2a,   /* brown      */
  0x7f7f7f,   /* gray50     */
  0x404040,   /* gray25     */
  0x0000ff,   /* blue1      */
  0x00ff00,   /* green1     */
  0xe0ffff,   /* light cyan */
  0xff0000,   /* red1       */
  0x9b30ff,   /* purple1    */
  0xffff00,   /* yellow     */
  0xffffff    /* white      */
  };

Int32 cursor_x;
Int32 cursor_y;
Int32 text_color;
Int32 g_color;

/* The GDI build left a colour outside the palette at the GC's default, black */
static UInt32 palette_rgb(Int32 color) {
  return (color >= 0 && color < 16) ? PALETTE[color] : PALETTE[CLR_BLACK];
  }

static void clear_buffer() {
  canvas_fill(screen,0,0,screen_width,screen_height,PALETTE[CLR_BLACK]);
  }

/* What the game drew since the last call reaches the window */
static void present() {
  Int32 x,y,width,height;
  if (canvas_take_dirty(screen,&x,&y,&width,&height)) rcs_invalidateViewport(viewport,x,y,width,height);
  rcs_presentViewport(viewport);
  }

void clrscr() {
  clear_buffer();
  }

void init_graph() {
  display = rcs_openDisplay(":0.0");
  window = rcs_createWindow(display,rcs_rootWindow(display),0,0,screen_width,screen_height);
  rcs_setWindowName(display,window,"Quest for the Unicorn");
  screen = canvas_create(screen_width,screen_height);
  saved = canvas_create(screen_width,screen_height);
  sprites = sprite_cache_create();
  font = terminal_font();
  if (screen == NULL || saved == NULL || sprites == NULL) exit(1);
  viewport = rcs_createViewport(display,window,canvas_pixels(screen),screen_width,screen_height);
  clear_buffer();
  rcs_showWindow(display,window);
  }

void saveScreen() {
  canvas_copy(saved,screen);
  }

void restoreScreen() {
  canvas_copy(screen,saved);
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
  if (screenshot_name(filename,"bmp")) canvas_save_bmp(screen,filename);
  }

void draw(Int32 x1,Int32 y1,Int32 x2,Int32 y2,Int32 clr) {
  canvas_line(screen,x1,y1,x2,y2,palette_rgb(clr));
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
  b2[1] = 0;
  for (i=0; i<strlen(buffer); i++) {
    x = cursor_x + text_offset_x;
    y = cursor_y + text_offset_y;
    b2[0] = buffer[i];
    canvas_fill(screen,x*8,y*19,8,19,PALETTE[CLR_BLACK]);
    canvas_text(screen,font,x * 8,10+y * 19,b2,palette_rgb(text_color));
    cursor_x++;
    if (cursor_x == 80) {
      cursor_x = 0;
      cursor_y++;
      if (cursor_y > 24) cursor_y = 24;
      }
    }
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

  present();
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
  present();
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
  present();
  }

/* Its callers pair it with their batches of plot() */
void close_display() {
  present();
  }

void plot(Int32 x,Int32 y,Int32 clr) {
  canvas_point(screen,x,y,palette_rgb(clr));
  }

void setfillstyle(Int32 st,Int32 clr) {
  g_color = clr;
  }

void bar(Int32 x1,Int32 y1,Int32 x2,Int32 y2) {
  canvas_fill(screen,x1,y1,x2-x1,y2-y1,palette_rgb(g_color));
  }

void setcolor(Int32 clr) {
  text_color = clr;
  g_color = clr;
  }

void settextstyle(Int32 a,Int32 b,Int32 c) {
  }

void outtext_xy(Int32 x,Int32 y,char* buffer) {
  canvas_text(screen,font,x*8,y*11,buffer,palette_rgb(g_color));
  }

void outtextxy(Int32 x,Int32 y,char* buffer) {
  canvas_text(screen,font,x,y,buffer,palette_rgb(g_color));
  }

/* The GDI build drew this one transparently too */
void outtextxyclr(Int32 x,Int32 y,char* buffer) {
  canvas_text(screen,font,x,y,buffer,palette_rgb(g_color));
  }

void putimage(Int32 x,Int32 y,char** img,Int32 typ) {
  canvas_sprite(screen,sprites,x,y,(const char* const*)img);
  }

void delay(Int32 tm) {
  present();
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


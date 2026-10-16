/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "header.h"
#include "terminal.h"
#include "platform.h"

/* What shows the game's frames and brings its keys */
static GAME_HOST*        host;

/* Drawing goes to the canvas only; present() hands it to the host when the game waits.
   saved is the copy saveScreen() keeps; sprites and font draw on either. */
static CANVAS*           screen;
static CANVAS*           saved;
static SPRITE_CACHE*     sprites;
static const GLYPH_FONT* font;

/* CLR_BLACK..CLR_WHITE in the Color Palette preference's palettes. X11 is the GDI build's, which resolved their X11
   names, so light red (red1) is red itself; DOS is the 16 colours of CGA, EGA and VGA. */
static const UInt32 X11_PALETTE[16] = {
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

static const UInt32 DOS_PALETTE[16] = {
  0x000000,   /* black         */
  0x0000aa,   /* blue          */
  0x00aa00,   /* green         */
  0x00aaaa,   /* cyan          */
  0xaa0000,   /* red           */
  0xaa00aa,   /* magenta       */
  0xaa5500,   /* brown         */
  0xaaaaaa,   /* light gray    */
  0x555555,   /* dark gray     */
  0x5555ff,   /* light blue    */
  0x55ff55,   /* light green   */
  0x55ffff,   /* light cyan    */
  0xff5555,   /* light red     */
  0xff55ff,   /* light magenta */
  0xffff55,   /* yellow        */
  0xffffff    /* white         */
  };

Int32 cursor_x;
Int32 cursor_y;
Int32 text_color;
Int32 g_color;

/* The colour in the palette the Color Palette preference picks. The GDI build left a colour outside the palette at the
   GC's default, black. */
static UInt32 palette_rgb(Int32 color) {
  const UInt32* palette;

  palette = (color_palette == 'D') ? DOS_PALETTE : X11_PALETTE;

  return (color >= 0 && color < 16) ? palette[color] : palette[CLR_BLACK];
  }

static void clear_buffer() {
  canvas_fill(screen,0,0,screen_width,screen_height,palette_rgb(CLR_BLACK));
  }

/* What the game drew since the last call reaches the window. Only a wait for a key or a delay calls it, so a screen
   still being drawn, such as a step's with the player erased, never shows. */
static void present() {
  host_present(host,screen);
  }

void clrscr() {
  clear_buffer();
  }

/* The window is already open; the game draws its first frame black */
void init_graph(GAME_HOST* game_host) {
  host = game_host;
  screen = canvas_create(screen_width,screen_height);
  saved = canvas_create(screen_width,screen_height);
  sprites = sprite_cache_create();
  font = terminal_font();
  if (screen == NULL || saved == NULL || sprites == NULL) exit(1);
  clear_buffer();
  present();
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
    canvas_fill(screen,x*8,y*19,8,19,palette_rgb(CLR_BLACK));
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

/* The window is closing or gone; without this the process lingers, music and all */
static void window_closed() {
  stop_music();
  exit(0);
  }

/* Ctrl+P and Ctrl+M waiting at the front of the queue are handled here, so a check for keys never stops on them */
static Int32 peek_key() {
  Int32 key;
  key = host_peek_key(host);
  while (key == KY_SCREENSHOT || key == KY_MUSIC) {
    host_read_key(host);
    if (key == KY_SCREENSHOT) screenshot();
      else toggle_music();
    key = host_peek_key(host);
    }
  if (key == HOST_KEY_CLOSED) window_closed();
  return key;
  }

Int32 keypressed() {
  if (strlen(currentMacro) > 0) return 1;

  present();
  return (peek_key() == HOST_KEY_NONE) ? 0 : -1;
  }

/* Throws away the keys typed ahead and the rest of a macro. It neither waits nor presents, so a screen still being
   drawn stays off the window. */
void drop_keys() {
  currentMacro[0] = 0;
  while (peek_key() != HOST_KEY_NONE) host_read_key(host);
  }

static Int32 next_key() {
  Int32 key;
  present();
  key = host_read_key(host);
  if (key == HOST_KEY_CLOSED) window_closed();
  return key;
  }

/* Ctrl+P and Ctrl+M work on every screen, so the game never sees them. ShowKeys prints every key it does see. */
Int32 readkey() {
  Int32 key;
  char  text[32];
  key = next_key();
  while (key == KY_SCREENSHOT || key == KY_MUSIC) {
    if (key == KY_SCREENSHOT) screenshot();
      else toggle_music();
    key = next_key();
    }
  if (show_keys == 'Y') {
    sprintf(text,"key=%d",key);
    msg(text,15);
    }
  return key;
  }

/* Nothing to do: the screen reaches the window at the game's next wait */
void flush() {
  }

/* Its callers pair it with their batches of plot(); as flush(), it leaves the screen to the next wait */
void close_display() {
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

/* Text on black, as txt_write() draws it, so spaces erase what was there */
void outtextxyclr(Int32 x,Int32 y,char* buffer) {
  canvas_fill(screen,x,y-10,8*(Int32)strlen(buffer),12,palette_rgb(CLR_BLACK));
  canvas_text(screen,font,x,y,buffer,palette_rgb(g_color));
  }

void putimage(Int32 x,Int32 y,char** img,Int32 typ) {
  canvas_sprite(screen,sprites,x,y,(const char* const*)img);
  }

/* A sprite at half its size, each pixel the average of a 2x2 block */
void put_half_image(Int32 x,Int32 y,char** img) {
  canvas_half_sprite(screen,sprites,x,y,(const char* const*)img);
  }

/* A sprite repeated across x1,y1 to x2,y2, right and bottom left out as bar() leaves them, its pixels counted from
   origin_x,origin_y */
void put_tiled_image(Int32 x1,Int32 y1,Int32 x2,Int32 y2,Int32 origin_x,Int32 origin_y,char** img) {
  canvas_tiled_sprite(screen,sprites,x1,y1,x2-x1,y2-y1,origin_x,origin_y,(const char* const*)img);
  }

void delay(Int32 tm) {
  present();
  host_sleep(host,tm);
  if (host_closed(host)) window_closed();
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


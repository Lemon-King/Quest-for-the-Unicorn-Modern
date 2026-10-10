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

#define MENU_LINE       22
#define MENU_COLUMN     36
#define MENU_NEW_GAME    0
#define MENU_LOAD_GAME   1
#define MENU_COUNT       2

static char* MENU_LABELS[MENU_COUNT] = { "New Game", "Load Game" };

#ifndef ASCII
typedef struct {
  Int32  x;
  Int32  y;
  Int32  size;
  Int32  color;
  char*  text;
  } TITLE_LINE;

/* The title block in Gothic, placed where the DOS release drew it on a 640x480 screen. */
static const TITLE_LINE TITLE_LINES[] = {
  { 275,  27, 4, 14, "Quest" },
  { 285,  82, 2,  2, "for the" },
  { 265, 110, 4, 14, "Unicorn" },
  { 300, 158, 2,  4, VERSION },
  { 245, 185, 3,  9, "by Mike Riley" }
  };

#define TITLE_LINE_COUNT ((Int32)(sizeof(TITLE_LINES) / sizeof(TITLE_LINE)))
#endif

static void show_text_title() {
  setcolor(14);
  gotoxy(38, 2); txt_write("Quest");
  setcolor(2);
  gotoxy(37, 4); txt_write("for the");
  setcolor(14);
  gotoxy(37, 6); txt_write("Unicorn");
  setcolor(4);
  gotoxy(37, 9); txt_write(VERSION);
  setcolor(9);
  gotoxy(34, 11); txt_write("by Mike Riley");
  }

#ifndef ASCII
static char show_gothic_title() {
  STROKE_FONT* font;
  Int32        i;
  Int32        left,top;

  font = gothic_font();
  if (font == NULL) return 'N';

  left = text_offset_x * 8;
  top = text_offset_y * 19;

  for (i=0; i<TITLE_LINE_COUNT; i++) {
    stroke_font_draw(font,left + TITLE_LINES[i].x,top + TITLE_LINES[i].y,TITLE_LINES[i].size,
                     TITLE_LINES[i].color,TITLE_LINES[i].text);
    }

  return 'Y';
  }
#endif

static void show_title_screen() {
  textmode(3);
#ifdef MHR
#include "./Mhr/title.c"
#else
#ifdef ASCII
  show_text_title();
#else
  if (show_gothic_title() != 'Y') show_text_title();
#endif
#endif
  setcolor(15);
  gotoxy(3, 17);
  txt_write("This program is copyright by Michael H. Riley and may be copied, modified,");
  gotoxy(3, 18);
  txt_write("and distributed so long as this title screen is not modified or removed.");
  gotoxy(3, 19);
  txt_write("This program may not be used for commercial purposes without the express");
  gotoxy(3, 20);
  txt_write("written permission from the author.");
  }

static void draw_menu_item(Int32 item, char selected) {
  textcolor(15);
  gotoxy(MENU_COLUMN - 4, MENU_LINE + item); txt_write((selected == 'Y') ? "-->" : "   ");

  textcolor((selected == 'Y') ? 14 : 15);
  gotoxy(MENU_COLUMN, MENU_LINE + item); txt_write(MENU_LABELS[item]);
  }

static Int32 choose_menu_item(Int32 item) {
  Int32 i;
  char  key;
  for (i=0; i<MENU_COUNT; i++) draw_menu_item(i, 'N');

  key = 0;
  while (key != 13) {
    draw_menu_item(item, 'Y');
    key = inkey();
    draw_menu_item(item, 'N');
    if (key == '8') item = (item + MENU_COUNT - 1) % MENU_COUNT;
    if (key == '2') item = (item + 1) % MENU_COUNT;
    }
  return item;
  }

/* The title screen for a game named on the command line. */
void title() {
  char   key;
  show_title_screen();
  gotoxy(28, 24);
  txt_write("Press <ENTER> to continue");

  key = ' ';
  while (key != 13) {
    key = inkey();
    }
  }

/* The title screen with the main menu. Returns 1 when Load Game put a saved game's name in loadname. */
Int32 title_menu() {
  Int32 item;
  Int32 loadgame;
  item = MENU_NEW_GAME;
  loadgame = 0;

  do {
    show_title_screen();
    item = choose_menu_item(item);
    if (item == MENU_LOAD_GAME) loadgame = load_menu();
    } while (item == MENU_LOAD_GAME && loadgame == 0);

  return loadgame;
  }


/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The name view mode and Floor Item Names show on the bottom line, right of the status icons and the compass */

#include "header.h"

#define NAME_LEFT     325     /* past the twelve status icons and the compass */
#define NAME_START    160     /* where a name starts, from the screen's right edge */
#define NAME_Y         15     /* the baseline, from the screen's bottom edge */
#define CELL_WIDTH      8
#define MAX_NAME      127

static Int32 name_x = 0;
static Int32 name_length = 0;

/* Clears the name, if one is showing */
void hide_bottom_name(void) {
  char blank[MAX_NAME+1];

  if (name_length == 0) return;

  sprintf(blank, "%*s", name_length, "");
  outtextxyclr(name_x, screen_height - NAME_Y, blank);
  name_length = 0;
  }

/* Shows name in color without its trailing spaces. A name too long to start in its place moves left, leaving one blank
   cell at the screen's edge. */
void show_bottom_name(char* name, Int32 color) {
  char  text[MAX_NAME+1];
  Int32 length;
  Int32 longest;

  hide_bottom_name();

  length = (Int32)strlen(name);
  while (length > 0 && name[length-1] == ' ') length--;

  longest = (screen_width - NAME_LEFT) / CELL_WIDTH - 1;
  if (longest > MAX_NAME) longest = MAX_NAME;
  if (length > longest) length = longest;
  if (length <= 0) return;

  strncpy(text, name, length);
  text[length] = 0;

  name_x = screen_width - NAME_START;
  if (name_x + CELL_WIDTH * (length + 1) > screen_width) name_x = screen_width - CELL_WIDTH * (length + 1);

  setcolor(color);
  outtextxyclr(name_x, screen_height - NAME_Y, text);
  name_length = length;
  }

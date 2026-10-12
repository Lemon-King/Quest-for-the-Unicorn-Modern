/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Lists that scroll, as on the Game Options screen: the rows in sight, their "more" markers and scroll bar, and the
   arrow at the selected row. */

#include "header.h"

#define MORE_WIDTH      6     /* "more ^"; the scroll bar takes the column after the next */
#define CELL_WIDTH      8     /* txt_write()'s cells, in pixels */
#define CELL_HEIGHT    19
#define THUMB_WIDTH     4

Int32 list_shows(SCROLL_LIST* list,Int32 row) {
  return (row >= list->top && row < list->top + list->shown);
  }

/* Scrolls the list until row shows. Returns 1 when it moved. */
Int32 list_scroll_to(SCROLL_LIST* list,Int32 row) {
  if (list_shows(list,row)) return 0;

  if (row < list->top) list->top = row;
    else list->top = row - list->shown + 1;

  return 1;
  }

Int32 list_line(SCROLL_LIST* list,Int32 row) {
  return list->line + 1 + row - list->top;
  }

/* A track beside the rows, with a thumb as long as the share of rows shown, placed by how far the list scrolled */
static void draw_scroll_bar(SCROLL_LIST* list) {
  Int32 x,y;
  Int32 height;
  Int32 thumb;
  Int32 offset;

  x = (text_offset_x + list->more_column + MORE_WIDTH) * CELL_WIDTH;
  y = (text_offset_y + list->line) * CELL_HEIGHT;
  height = list->shown * CELL_HEIGHT;

  thumb = height * list->shown / list->count;
  if (thumb < CELL_HEIGHT) thumb = CELL_HEIGHT;
  offset = (height - thumb) * list->top / (list->count - list->shown);

  setfillstyle(1,0);
  bar(x,y,x + CELL_WIDTH,y + height);

  setfillstyle(1,8);
  bar(x + CELL_WIDTH / 2,y,x + CELL_WIDTH / 2 + 1,y + height);
  x += (CELL_WIDTH - THUMB_WIDTH) / 2;
  bar(x,y + offset,x + THUMB_WIDTH,y + offset + thumb);
  }

/* Marks rows scrolled out of sight above or below a list that scrolls, and draws its scroll bar */
void draw_list_more(SCROLL_LIST* list) {
  if (list->count <= list->shown) return;

  textcolor(8);

  gotoxy(list->more_column,list->line);
  txt_write((list->top > 0) ? "more ^" : "      ");

  gotoxy(list->more_column,list->line + list->shown + 1);
  txt_write((list->top + list->shown < list->count) ? "more v" : "      ");

  draw_scroll_bar(list);
  }

void draw_list_arrow(Int32 column,Int32 line,Int32 selected) {
  textcolor(15);
  gotoxy(column,line);
  txt_write(selected ? "-->" : "   ");
  }

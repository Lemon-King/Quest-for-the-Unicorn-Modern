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

static Int32 is_panel(SCROLL_LIST* list) {
  return (list->row_height > 0);
  }

static Int32 row_height(SCROLL_LIST* list) {
  return is_panel(list) ? list->row_height : CELL_HEIGHT;
  }

/* The pixel at the top left of a cell */
static void cell_corner(SCROLL_LIST* list,Int32 column,Int32 line,Int32* x,Int32* y) {
  if (is_panel(list)) {
    *x = list->x + (column - 1) * CELL_WIDTH;
    *y = list->y + (line - 1) * list->row_height;
    }
  else {
    *x = (text_offset_x + column - 1) * CELL_WIDTH;
    *y = (text_offset_y + line - 1) * CELL_HEIGHT;
    }
  }

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

/* Writes text on its cells, clearing them first, as txt_write() does on the text grid */
void list_text(SCROLL_LIST* list,Int32 column,Int32 line,Int32 color,char* text) {
  Int32 x,y;

  if (!is_panel(list)) {
    textcolor(color);
    gotoxy(column,line);
    txt_write(text);
    return;
    }

  cell_corner(list,column,line,&x,&y);
  setfillstyle(1,0);
  bar(x,y,x + CELL_WIDTH * (Int32)strlen(text),y + list->row_height);
  setcolor(color);
  outtextxy(x,y + 10,text);
  }

/* A track beside the rows, with a thumb as long as the share of rows shown, placed by how far the list scrolled */
static void draw_scroll_bar(SCROLL_LIST* list) {
  Int32 x,y;
  Int32 height;
  Int32 thumb;
  Int32 offset;

  cell_corner(list,list->more_column + MORE_WIDTH + 1,list->line + 1,&x,&y);
  height = list->shown * row_height(list);

  thumb = height * list->shown / list->count;
  if (thumb < row_height(list)) thumb = row_height(list);
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

  list_text(list,list->more_column,list->line,8,(list->top > 0) ? "more ^" : "      ");
  list_text(list,list->more_column,list->line + list->shown + 1,8,
            (list->top + list->shown < list->count) ? "more v" : "      ");

  draw_scroll_bar(list);
  }

/* The columns the arrow takes: a panel has room for no more than ">" */
Int32 list_arrow_width(SCROLL_LIST* list) {
  return is_panel(list) ? 1 : 3;
  }

void draw_list_arrow(SCROLL_LIST* list,Int32 column,Int32 line,Int32 selected) {
  if (is_panel(list)) list_text(list,column,line,15,selected ? ">" : " ");
    else list_text(list,column,line,15,selected ? "-->" : "   ");
  }

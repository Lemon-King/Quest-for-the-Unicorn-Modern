/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Menus picked from with a cursor, for the Cursor Menus preference: 8/2 move it a row, wrapping at either end, 9/3 a
   page and 7/1 to the first or last row, and the rows scroll to keep it in sight. */

#include "header.h"

/* The label's column, a space after the arrow */
static Int32 label_column(MENU* menu) {
  return menu->column + list_arrow_width(&menu->rows) + 1;
  }

static void draw_shown_rows(MENU* menu) {
  Int32 row;

  for (row=menu->rows.top; row<menu->rows.count && list_shows(&menu->rows,row); row++)
    menu->draw_row(menu,row,list_line(&menu->rows,row));

  draw_list_more(&menu->rows);
  }

/* Keeps the cursor on one of count rows, and the rows scrolled no further than they need */
void fit_menu(MENU* menu,Int32 count) {
  menu->rows.count = count;

  if (menu->row >= count) menu->row = count - 1;
  if (menu->row < 0) menu->row = 0;

  if (menu->rows.top > count - menu->rows.shown) menu->rows.top = count - menu->rows.shown;
  if (menu->rows.top < 0) menu->rows.top = 0;
  }

void draw_menu(MENU* menu) {
  if (menu->keys != NULL) {
    textcolor(3);
    gotoxy(41 - (Int32)strlen(menu->keys) / 2,menu->keys_line);
    txt_write(menu->keys);
    }

  draw_shown_rows(menu);
  }

/* A row's label after the arrow, padded or cut to its value so the row covers the one it replaces, then the value */
void draw_menu_text_in(MENU* menu,Int32 line,char* label,char* value,Int32 color) {
  char  text[81];
  Int32 width;

  width = menu->value_column - label_column(menu);
  sprintf(text,"%-*.*s",width,width,label);

  list_text(&menu->rows,label_column(menu),line,color,text);
  list_text(&menu->rows,menu->value_column,line,color,value);
  }

void draw_menu_text(MENU* menu,Int32 line,char* label,char* value) {
  draw_menu_text_in(menu,line,label,value,menu->color);
  }

/* Shows the cursor and reads a key, moving the cursor for the keys that move it. Returns the key. */
char menu_key(MENU* menu) {
  Int32 last;
  char  key;

  if (menu->rows.count == 0) return inkey();

  draw_list_arrow(&menu->rows,menu->column,list_line(&menu->rows,menu->row),1);
  key = inkey();
  draw_list_arrow(&menu->rows,menu->column,list_line(&menu->rows,menu->row),0);

  last = menu->rows.count - 1;
  switch (key) {
    case '8': menu->row = (menu->row > 0) ? menu->row - 1 : last; break;
    case '2': menu->row = (menu->row < last) ? menu->row + 1 : 0; break;
    case '9': menu->row -= menu->rows.shown; break;
    case '3': menu->row += menu->rows.shown; break;
    case '7': menu->row = 0; break;
    case '1': menu->row = last; break;
    }

  if (menu->row < 0) menu->row = 0;
  if (menu->row > last) menu->row = last;

  if (list_scroll_to(&menu->rows,menu->row)) draw_shown_rows(menu);

  return key;
  }

/* The row picked with ENTER, or -1 when ESC backs out */
Int32 menu_choice(MENU* menu) {
  char key;

  key = 0;
  while (key != 13 && key != 27) key = menu_key(menu);

  if (key == 27 || menu->rows.count == 0) return -1;

  return menu->row;
  }

/* The key that picks the chosen row in a numbered menu, '1' for the first, or ESC */
char menu_number(MENU* menu) {
  Int32 row;

  row = menu_choice(menu);

  return (row >= 0) ? (char)('1' + row) : 27;
  }

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

#define CENTRE_COLUMN   41    /* text starting half its length before this is centred on the 80 columns */

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

void write_centred(Int32 line,Int32 color,char* text) {
  textcolor(color);
  gotoxy(CENTRE_COLUMN - (Int32)strlen(text) / 2,line);
  txt_write(text);
  }

/* The keys a menu takes, centred on line */
void draw_menu_keys(char* keys,Int32 line) {
  write_centred(line,3,keys);
  }

/* Clears a menu's key tips while a sub menu or question shows, as they are not its keys */
void hide_menu_keys(MENU* menu) {
  char blank[81];

  sprintf(blank,"%*s",(Int32)strlen(menu->keys),"");
  draw_menu_keys(blank,menu->keys_line);
  }

/* A question centred in place of a menu's key tips */
void show_menu_question(MENU* menu,Int32 color,char* question) {
  hide_menu_keys(menu);
  write_centred(menu->keys_line,color,question);
  }

/* A question about an item, centred in place of a menu's key tips, with only the item's name in its rarity colour */
void show_item_question(MENU* menu,Int32 color,char* before,OBJECT_REC* item,char* name,char* after) {
  Int32 length;

  length = (Int32)(strlen(before) + strlen(name) + strlen(after));
  hide_menu_keys(menu);
  gotoxy(CENTRE_COLUMN - length / 2,menu->keys_line);
  write_named(before,item,name,after,color);
  }

void draw_menu(MENU* menu) {
  if (menu->keys != NULL) draw_menu_keys(menu->keys,menu->keys_line);

  draw_shown_rows(menu);
  }

/* The columns a label has before its value */
Int32 menu_label_width(MENU* menu) {
  return menu->value_column - label_column(menu);
  }

/* A row's label after the arrow, padded or cut to its value so the row covers the one it replaces, then the value */
static void draw_row_text(MENU* menu,Int32 line,char* label,Int32 label_color,char* value,Int32 value_color) {
  char  text[81];
  Int32 width;

  width = menu_label_width(menu);
  sprintf(text,"%-*.*s",width,width,label);

  list_text(&menu->rows,label_column(menu),line,label_color,text);
  list_text(&menu->rows,menu->value_column,line,value_color,value);
  }

void draw_menu_text_in(MENU* menu,Int32 line,char* label,char* value,Int32 color) {
  draw_row_text(menu,line,label,color,value,color);
  }

/* A row naming an item: the name in its rarity colour, name_color, and the value in the menu's colour */
void draw_menu_item(MENU* menu,Int32 line,char* name,Int32 name_color,char* value) {
  draw_row_text(menu,line,name,name_color,value,menu->color);
  }

void draw_menu_text(MENU* menu,Int32 line,char* label,char* value) {
  draw_menu_text_in(menu,line,label,value,menu->color);
  }

/* A row of a menu whose screen is its labels */
void draw_menu_choice(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,((char**)menu->screen)[row],"");
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

/* Asks with the choices as a menu from column and line down, in place of a question, and erases them once one is
   picked; column 0 centres the menu. The cursor starts on row. Returns the row picked, or -1 for ESC. */
Int32 ask_choice(Int32 column,Int32 line,Int32 color,char** choices,Int32 count,Int32 row) {
  MENU  menu = { .rows = { .shown = count, .line = line - 1 }, .row = row, .column = column, .color = color,
                 .draw_row = draw_menu_choice, .screen = choices };
  char  blank[81];
  Int32 width;
  Int32 i;

  width = 0;
  for (i=0; i<count; i++)
    if ((Int32)strlen(choices[i]) > width) width = (Int32)strlen(choices[i]);

  if (column == 0) menu.column = CENTRE_COLUMN - (list_arrow_width(&menu.rows) + 1 + width) / 2;
  menu.value_column = label_column(&menu) + width;
  fit_menu(&menu,count);
  draw_menu(&menu);

  row = menu_choice(&menu);

  sprintf(blank,"%*s",menu.value_column - menu.column,"");
  for (i=0; i<count; i++) list_text(&menu.rows,menu.column,line + i,color,blank);

  return row;
  }

/* The key a Y/N question would read: 'Y' when the action is picked, 'N' for cancel or ESC */
char ask_action(Int32 column,Int32 line,Int32 color,char* action,char* cancel,Int32 row) {
  char* choices[2];

  choices[0] = action;
  choices[1] = cancel;

  return (ask_choice(column,line,color,choices,2,row) == 0) ? 'Y' : 'N';
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Stored items with the Cursor Menus preference: the pack, a house and the portable hole list their slots in one
   numbered list beside the selected item's card, and ENTER offers what can be done with the slot. */

#include "header.h"

#define STORAGE_ROWS    18
#define MORE_COLUMN     39      /* the list's "more" markers, after the names */
#define ACTION_COLUMN   48      /* the actions for a slot show under its card */
#define ACTION_LINE     16
#define STORAGE_KEYS    "(8/2) Select   (ENTER) Actions   (ESC) Leave"
#define SHORTCUT_KEYS   "Ii xX"

/* A slot, numbered so the empty ones show */
static void draw_storage_row(MENU* menu,Int32 row,Int32 line) {
  STORAGE*    storage;
  OBJECT_REC* item;
  char        label[8];
  char        name[128];
  char        text[MORE_COLUMN];
  Int32       width;

  storage = menu->screen;
  item = &(storage->slots[row]);
  width = MORE_COLUMN - menu->value_column;

  sprintf(label,"%*d:",storage->digits,row + 1);
  list_item_name(item,width,name);
  sprintf(text,"%-*s",width,name);

  draw_menu_text(menu,line,label,"");
  list_text(&menu->rows,menu->value_column,line,item_color(item),text);
  }

/* Moves the item at the cursor to the slot picked with the cursor, swapping it with whatever is there */
static void move_stored_item(MENU* menu) {
  STORAGE*   storage;
  OBJECT_REC temp;
  char       name[128];
  char       question[160];
  Int32      from;
  Int32      to;

  storage = menu->screen;
  from = menu->row;
  list_item_name(&(storage->slots[from]),MORE_COLUMN,name);
  sprintf(question,"Move %s to which slot?",name);
  show_menu_question(menu,15,question);

  to = menu_choice(menu);
  if (to >= 0) {
    temp = storage->slots[from];
    storage->slots[from] = storage->slots[to];
    storage->slots[to] = temp;
    }
  else menu->row = from;

  list_scroll_to(&menu->rows,menu->row);
  }

/* Sets menu up to list count slots of storage */
void open_storage(MENU* menu,STORAGE* storage,Int32 count) {
  char number[12];

  sprintf(number,"%d",count);
  storage->digits = (Int32)strlen(number);

  memset(menu,0,sizeof(MENU));
  menu->rows.count = count;
  menu->rows.shown = STORAGE_ROWS;
  menu->rows.line = 2;
  menu->rows.more_column = MORE_COLUMN;
  menu->column = 2;
  menu->value_column = menu->column + list_arrow_width(&menu->rows) + 1 + storage->digits + 2;
  menu->color = 2;
  menu->keys = STORAGE_KEYS;
  menu->keys_line = 25;
  menu->draw_row = draw_storage_row;
  menu->screen = storage;
  }

/* Shows the selected slot's card and reads a key with the cursor. Returns ENTER, ESC or a key the screen takes
   without the cursor (I, space, X), or 0 for the keys the cursor took. */
char storage_key(MENU* menu) {
  STORAGE* storage;
  char     key;

  storage = menu->screen;
  draw_card_panel(&(storage->slots[menu->row]),menu->rows.line + 1,menu->rows.line + menu->rows.shown);
  key = menu_key(menu);

  if (key == 13 || key == 27) return key;
  if (key != 0 && strchr(SHORTCUT_KEYS,key) != NULL) return key;

  return 0;
  }

/* The actions for the selected slot: main, the screen's own action for ENTER (none when NULL), then picking up,
   putting down or swapping the item in the air (space), moving the item, and Cancel. Moves the item itself.
   Returns the key that does the action without the cursor, 'M' after a move, or 0. */
char storage_action(MENU* menu,char* main) {
  STORAGE*    storage;
  OBJECT_REC* item;
  char*       labels[4];
  char        keys[4];
  Int32       count;
  Int32       picked;

  storage = menu->screen;
  item = &(storage->slots[menu->row]);
  count = 0;

  if (main != NULL) {
    labels[count] = main;
    keys[count++] = 13;
    }
  if (item->otype != O_NONE || storage->inair->otype != O_NONE) {
    if (storage->inair->otype == O_NONE) labels[count] = "Pick Up";
      else labels[count] = (item->otype == O_NONE) ? "Put Here" : "Swap with In Air";
    keys[count++] = ' ';
    }
  if (item->otype != O_NONE) {
    labels[count] = "Move";
    keys[count++] = 'M';
    }
  labels[count] = "Cancel";
  keys[count++] = 0;

  picked = ask_choice(ACTION_COLUMN,ACTION_LINE,15,labels,count,0);
  if (picked < 0) return 0;

  if (keys[picked] == 'M') move_stored_item(menu);
  return keys[picked];
  }

/* The item a house or the portable hole holds in the air, on line */
void show_in_air(OBJECT_REC* in_air,Int32 line) {
  char buffer[128];

  textcolor(CLR_WHITE);
  gotoxy(20,line); txt_write("In Air: ");
  if (in_air == NULL || in_air->otype == O_NONE) return;

  sprintf(buffer,"%s",get_name(in_air));
  shorten_name(buffer,30);
  itemColor(in_air);
  txt_write(buffer);
  }

/* ENTER's action in a house or the portable hole, which trades the slot's item with the inventory */
char* transfer_action(OBJECT_REC* item) {
  return (item->otype != O_NONE) ? "Take Out" : "Store Item";
  }

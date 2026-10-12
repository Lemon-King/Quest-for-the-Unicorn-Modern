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
#define STORAGE_KEYS    "(8/2) Select   (ENTER) Actions   (S) Sort   (ESC) Leave"
#define SHORTCUT_KEYS   "Ii xX"

#define SORT_NAME           0
#define SORT_NAME_REVERSE   1
#define SORT_TYPE           2
#define SORT_RARITY         3
#define SORT_VALUE          4
#define SORT_QUANTITY       5
#define NUM_SORTS           6

/* The orders, then Cancel */
static char* sort_names[NUM_SORTS + 1] = { "Name", "Name Reverse", "Type", "Rarity", "Value", "Quantity", "Cancel" };

/* A slot's place in a sort. Rarity, value and quantity (the charges a name shows, or one) are known only for
   identified items; the others sort after them. */
typedef struct {
  Int32  slot;
  Int32  empty;
  char   name[64];
  char*  kind;
  Int32  rarity;
  double value;
  Int32  quantity;
  } SORT_KEY;

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
  Int32      from;
  Int32      to;

  storage = menu->screen;
  from = menu->row;
  list_item_name(&(storage->slots[from]),MORE_COLUMN,name);
  show_item_question(menu,15,"Move ",&(storage->slots[from]),name," to which slot?");

  to = menu_choice(menu);
  if (to >= 0) {
    temp = storage->slots[from];
    storage->slots[from] = storage->slots[to];
    storage->slots[to] = temp;
    }
  else menu->row = from;

  list_scroll_to(&menu->rows,menu->row);
  }

static void make_sort_key(OBJECT_REC* item,Int32 slot,SORT_KEY* key) {
  Int32 known;

  known = (item->flags & OF_IDENT) != 0;

  key->slot = slot;
  key->empty = (item->otype == O_NONE);
  key->kind = item_kind(item);
  key->rarity = known ? abs(item->rarity) : INT32_MAX;
  key->value = known ? item_cost(item) : -1.0;
  key->quantity = !known ? 0 : (item->charges > 0) ? item->charges : 1;
  list_item_name(item,sizeof(key->name) - 1,key->name);
  }

/* Below zero when a sorts before b: empty slots last, then the order's own key, then the name */
static Int32 compare_keys(SORT_KEY* a,SORT_KEY* b,Int32 order) {
  Int32 by_name;
  Int32 by_kind;

  if (a->empty || b->empty) return a->empty - b->empty;

  by_name = _stricmp(a->name,b->name);
  by_kind = _stricmp(a->kind,b->kind);

  if (order == SORT_NAME_REVERSE) return -by_name;
  if (order == SORT_TYPE && by_kind != 0) return by_kind;
  if (order == SORT_RARITY && a->rarity != b->rarity) return (a->rarity < b->rarity) ? -1 : 1;
  if (order == SORT_VALUE && a->value != b->value) return (a->value > b->value) ? -1 : 1;
  if (order == SORT_QUANTITY && a->quantity != b->quantity) return (a->quantity > b->quantity) ? -1 : 1;

  return by_name;
  }

/* Sorts the slots listed in the order picked, keeping the order of equal items. Returns 1 when they were sorted. */
static Int32 sort_storage(MENU* menu) {
  STORAGE*    storage;
  SORT_KEY*   keys;
  OBJECT_REC* sorted;
  SORT_KEY    key;
  Int32       count;
  Int32       order;
  Int32       i;
  Int32       j;

  storage = menu->screen;
  count = menu->rows.count;

  show_menu_question(menu,15,"Sort the slots by?");
  order = ask_choice(ACTION_COLUMN,ACTION_LINE,15,sort_names,NUM_SORTS + 1,0);
  if (order < 0 || order == NUM_SORTS) return 0;

  keys = malloc(count * sizeof(SORT_KEY));
  sorted = malloc(count * sizeof(OBJECT_REC));
  if (keys == NULL || sorted == NULL) {
    free(keys);
    free(sorted);
    return 0;
    }

  for (i=0; i<count; i++) make_sort_key(&(storage->slots[i]),i,&(keys[i]));

  for (i=1; i<count; i++) {
    key = keys[i];
    for (j=i; j>0 && compare_keys(&key,&(keys[j - 1]),order) < 0; j--) keys[j] = keys[j - 1];
    keys[j] = key;
    }

  for (i=0; i<count; i++) sorted[i] = storage->slots[keys[i].slot];
  memcpy(storage->slots,sorted,count * sizeof(OBJECT_REC));

  free(keys);
  free(sorted);

  menu->row = 0;
  menu->rows.top = 0;
  return 1;
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

/* Shows the selected slot's card and reads a key with the cursor; S sorts the slots. Returns ENTER, ESC or a key the
   screen takes without the cursor (I, space, X), STORAGE_MOVED after a sort, or 0 for the keys the cursor took. */
char storage_key(MENU* menu) {
  STORAGE* storage;
  char     key;

  storage = menu->screen;
  draw_card_panel(&(storage->slots[menu->row]),menu->rows.line + 1,menu->rows.line + menu->rows.shown);
  key = menu_key(menu);

  if (key == 's' || key == 'S') {
    if (sort_storage(menu)) return STORAGE_MOVED;

    draw_menu_keys(menu->keys,menu->keys_line);
    return 0;
    }
  if (key == 13 || key == 27) return key;
  if (key != 0 && strchr(SHORTCUT_KEYS,key) != NULL) return key;

  return 0;
  }

/* The actions for the selected slot: main, the screen's own action for ENTER (none when NULL), then picking up,
   putting down or swapping the item in the air (space), moving the item, and Cancel. Moves the item itself.
   Returns the key that does the action without the cursor, STORAGE_MOVED after a move, or 0. */
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
    keys[count++] = STORAGE_MOVED;
    }
  labels[count] = "Cancel";
  keys[count++] = 0;

  picked = ask_choice(ACTION_COLUMN,ACTION_LINE,15,labels,count,0);
  if (picked < 0) return 0;

  if (keys[picked] == STORAGE_MOVED) move_stored_item(menu);
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

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

#define PACK_SLOTS      32
#define PACK_ROWS       18
#define NAME_WIDTH      29      /* a name in the list, which ends before its "more" markers */
#define ACTION_COLUMN   48      /* the actions for a slot show under its card */
#define ACTION_LINE     16
#define PACK_KEYS       "(8/2) Select   (ENTER) Actions   (ESC) Leave"
#define SHORTCUT_KEYS   "Ii xX"

/* A slot of the pack, numbered so the empty ones show */
static void draw_pack_row(MENU* menu,Int32 row,Int32 line) {
  OBJECT_REC* item;
  char        label[8];
  char        name[128];
  char        text[NAME_WIDTH + 1];

  item = &(player.pack[row + 1]);
  sprintf(label,"%2d:",row + 1);
  list_item_name(item,NAME_WIDTH,name);
  sprintf(text,"%-*s",NAME_WIDTH,name);

  draw_menu_text(menu,line,label,"");
  list_text(&menu->rows,menu->value_column,line,item_color(item),text);
  }

void unpack_top(OBJECT_REC in_air,MENU* menu) {
  Int32 i;
  char buffer[80];
  textmode(3);
  textcolor(15); gotoxy(33,1); txt_write("Pack Inventory");
  if (cursor_menus == 'Y') draw_menu(menu);
  else {
    gotoxy(13,3);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("ESC");
    textcolor(6); txt_write(">Exit, <");
    textcolor(14); txt_write("ENTER");
    textcolor(6); txt_write(">Take Item, <");
    textcolor(14); txt_write("Arrows");
    textcolor(6); txt_write(">Select Item <");
    textcolor(14); txt_write("I");
    textcolor(6); txt_write(">nfo");
    gotoxy(12,4);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("7");
    textcolor(6); txt_write("> Exchange up, <");
    textcolor(14); txt_write("1");
    textcolor(6); txt_write(">Exchange down, <");
    textcolor(14); txt_write("space");
    textcolor(6); txt_write("> Exchange In Air");
    textcolor(3);
    for (i=1; i<=16; i++) {
      if (player.pack[i].otype != O_NONE) {
        gotoxy(3,i+5);
        sprintf(buffer,"%s",get_name(&(player.pack[i])));
        if (strlen(buffer) > 36) buffer[36] = 0;
        itemColor(&(player.pack[i]));
        txt_write(buffer);
        }
      if (player.pack[i+16].otype != O_NONE) {
        gotoxy(43,i+5);
        sprintf(buffer,"%s",get_name(&(player.pack[i+16])));
        if (strlen(buffer) > 36) buffer[36] = 0;
        itemColor(&(player.pack[i+16]));
        txt_write(buffer);
        }
      }
    }
  gotoxy(23, 23); textcolor(CLR_WHITE); txt_write("In Air: ");
  if (in_air.otype != O_NONE) {
    sprintf(buffer,"%s",get_name(&in_air));
    if (strlen(buffer) > 36) buffer[36] = 0;
    itemColor(&in_air);
    gotoxy(31,23);
    txt_write(buffer);
    }
  }

/* Moves the item at the cursor to the slot picked with the cursor, swapping it with whatever is there */
static void move_pack_item(MENU* menu,OBJECT_REC inair) {
  OBJECT_REC temp;
  char       name[128];
  char       question[160];
  Int32      from;
  Int32      to;

  from = menu->row;
  list_item_name(&(player.pack[from + 1]),NAME_WIDTH,name);
  sprintf(question,"Move %s to which slot?",name);
  show_menu_question(menu,15,question);

  to = menu_choice(menu);
  if (to >= 0) {
    temp = player.pack[from + 1];
    player.pack[from + 1] = player.pack[to + 1];
    player.pack[to + 1] = temp;
    }
  else menu->row = from;

  list_scroll_to(&menu->rows,menu->row);
  unpack_top(inair,menu);
  }

/* The actions for pack slot index, each as the key that does it without the cursor, or 'M' to move it; the item is
   equipped into inventory slot, taken into the air for slot 0, or chosen for a negative slot. Returns the key
   picked, or 0. */
static char pick_pack_action(Int32 index,Int32 slot,OBJECT_REC* inair) {
  OBJECT_REC* item;
  char*       labels[4];
  char        keys[4];
  Int32       count;
  Int32       picked;

  item = &(player.pack[index]);
  count = 0;

  if (item->otype != O_NONE && (slot <= 0 || inv_valid(item,slot) == 'Y')) {
    if (slot > 0) labels[count] = "Equip";
      else labels[count] = (slot == 0) ? "Take" : "Choose";
    keys[count++] = 13;
    }
  if (item->otype != O_NONE || inair->otype != O_NONE) {
    if (inair->otype == O_NONE) labels[count] = "Pick Up";
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

  return (picked >= 0) ? keys[picked] : 0;
  }

/* A key with the Cursor Menus preference: the cursor picks the slot, shown beside its card, and ENTER an action for
   it, returned as the key that does it without the cursor. Returns 0 for the keys the cursor took. */
static char pack_key(MENU* menu,Int32 slot,OBJECT_REC* inair) {
  char key;

  draw_card_panel(&(player.pack[menu->row + 1]),menu->rows.line + 1,menu->rows.line + menu->rows.shown);
  key = menu_key(menu);

  if (key == 13) {
    key = pick_pack_action(menu->row + 1,slot,inair);
    if (key != 'M') return key;

    move_pack_item(menu,*inair);
    return 0;
    }
  if (key == 27 || (key != 0 && strchr(SHORTCUT_KEYS,key) != NULL)) return key;

  return 0;
  }

OBJECT_REC* unpack(Int32 slot) {
  char flag;
  char key;
  Int32  row;
  Int32  col;
  Int32  i;
  MENU   menu = { .rows = { .count = PACK_SLOTS, .shown = PACK_ROWS, .line = 2, .more_column = 39 }, .column = 2,
                  .value_column = 10, .color = 2, .keys = PACK_KEYS, .keys_line = 25, .draw_row = draw_pack_row };
  textmode(3);
  row = 0;
  col = 0;
  OBJECT_REC temp;
  OBJECT_REC inair;
  Int32 item1;
  Int32 item2;
/*
  textcolor(15); gotoxy(33,1); txt_write("Pack Inventory");
  gotoxy(17,3);
  textcolor(6);
  txt_write("<");
  textcolor(14); txt_write("ESC");
  textcolor(6); txt_write(">Exit, <");
  textcolor(14); txt_write("ENTER");
  textcolor(6); txt_write(">Take Item, <");
  textcolor(14); txt_write("Arrows");
  textcolor(6); txt_write(">Select Item");
  textcolor(3);
  for (i=1; i<=16; i++) {
    if (player.pack[i].otype != O_NONE) {
      gotoxy(3,i+5);
      sprintf(buffer,"%s",get_name(&(player.pack[i])));
      itemColor(&(player.pack[i]));
      txt_write(buffer);
      }
    if (player.pack[i+16].otype != O_NONE) {
      gotoxy(43,i+5);
      sprintf(buffer,"%s",get_name(&(player.pack[i+16])));
      itemColor(&(player.pack[i+16]));
      txt_write(buffer);
      }
    }
*/
  inair.otype = O_NONE;
  unpack_top(inair,&menu);
  flag = ' ';
  while (flag == ' ') {
    if (cursor_menus == 'Y') {
      key = pack_key(&menu,slot,&inair);
      row = menu.row % 16;
      col = menu.row / 16;
      }
    else {
      textcolor(14);
      gotoxy(1+col*40,row+6);
      txt_write("->");
      key = inkey();
      textcolor(14);
      gotoxy(1+col*40,row+6);
      txt_write("  ");
      }
    if (key == 27) flag = 'N';
    if (key == 13 && player.pack[row+1+col*16].otype != O_NONE) flag = 'Y';
    if ((key == 'i' || key == 'I') && player.pack[row+1+col*16].otype != O_NONE) {
      data_card(&(player.pack[row+1+col*16]));
      unpack_top(inair,&menu);
      }
    if (key == '2' || key == '.' || key == '>') {
      row++;
      if (row > 15) row = 0;
      }
    if (key == '8' || key == ',' || key == '<') {
      row--;
      if (row < 0) row = 15;
      }
    if (key == '6' || key == '4') {
      col = (col == 0) ? 1 : 0;
      }
    if (key == '7') {
      item1 = row + 1 + (col * 16);
      item2 = item1-1;
      if (item2 < 1) item2 = 32;
      temp = player.pack[item1];
      player.pack[item1] = player.pack[item2];
      player.pack[item2] = temp;
      col = (item2 - 1) / 16;
      item2 -= col * 16;
      row = item2 - 1;
      unpack_top(inair,&menu);
      }
    if (key == '1') {
      item1 = row + 1 + (col * 16);
      item2 = item1+1;
      if (item2 > 32) item2 = 1;
      temp = player.pack[item1];
      player.pack[item1] = player.pack[item2];
      player.pack[item2] = temp;
      col = (item2 - 1) / 16;
      item2 -= col * 16;
      row = item2 - 1;
      unpack_top(inair,&menu);
      }
    if (key == ' ' || key == 'x' || key == 'X') {
      item1 = row + 1 + (col * 16);
      temp = player.pack[item1];
      player.pack[item1] = inair;
      inair = temp;
      unpack_top(inair,&menu);
      }
    }
  if (inair.otype != O_NONE) {
    for (i=1; i<33; i++)
      if (player.pack[i].otype == O_NONE) {
        player.pack[i] = inair;
        i = 99;
        }
    }
  textmode(3);
  if (flag == 'Y') {
    if (slot > 0 && inv_valid(&(player.pack[row+1+col*16]),slot) == 'Y') {
      temp = player.invent[slot];
      player.invent[slot] = player.pack[row+1+col*16];
      player.pack[row+1+col*16] = temp;
      }
    if (slot == 0) {
      temp = in_air;
      in_air = player.pack[row+1+col*16];
      player.pack[row+1+col*16] = temp;
      }
    return &(player.pack[row+1+col*16]);
    }
  else return NULL;
  }


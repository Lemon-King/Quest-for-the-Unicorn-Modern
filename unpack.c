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

void unpack_top(OBJECT_REC in_air,MENU* menu) {
  Int32 i;
  char buffer[80];
  textmode(3);
  textcolor(15); gotoxy(33,1); txt_write("Pack Inventory");
  if (improved_interface == 'Y') draw_menu(menu);
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
        marked_item_name(&(player.pack[i]),buffer);
        if (strlen(buffer) > 36) buffer[36] = 0;
        itemColor(&(player.pack[i]));
        txt_write(buffer);
        }
      if (player.pack[i+16].otype != O_NONE) {
        gotoxy(43,i+5);
        marked_item_name(&(player.pack[i+16]),buffer);
        if (strlen(buffer) > 36) buffer[36] = 0;
        itemColor(&(player.pack[i+16]));
        txt_write(buffer);
        }
      }
    }
  gotoxy(23, 23); textcolor(CLR_WHITE); txt_write("In Air: ");
  if (in_air.otype != O_NONE) {
    marked_item_name(&in_air,buffer);
    if (strlen(buffer) > 36) buffer[36] = 0;
    itemColor(&in_air);
    gotoxy(31,23);
    txt_write(buffer);
    }
  }

/* ENTER's action on a pack item: equip it into inventory slot, take it into the air for slot 0, or choose it for a
   negative slot; none for an empty slot, or an item that inventory slot does not take */
static char* pack_action(OBJECT_REC* item,Int32 slot) {
  if (item->otype == O_NONE) return NULL;
  if (slot > 0) return (inv_valid(item,slot) == 'Y') ? "Equip" : NULL;

  return (slot == 0) ? "Take" : "Choose";
  }

OBJECT_REC* unpack(Int32 slot) {
  char flag;
  char key;
  Int32  row;
  Int32  col;
  Int32  i;
  OBJECT_REC inair;
  STORAGE    storage = { .slots = &(player.pack[1]), .inair = &inair };
  MENU       menu;
  textmode(3);
  row = 0;
  col = 0;
  OBJECT_REC temp;
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
  open_storage(&menu,&storage,PACK_SLOTS);
  unpack_top(inair,&menu);
  flag = ' ';
  while (flag == ' ') {
    if (improved_interface == 'Y') {
      key = storage_key(&menu);
      if (key == 13) key = storage_action(&menu,pack_action(&(player.pack[menu.row + 1]),slot));
      if (key == STORAGE_MOVED) unpack_top(inair,&menu);
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


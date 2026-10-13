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

/* The base of the last page. Without the House Storage change the pages stop at 440, so slots 480-499 never show. */
static Int32 last_base() {
  return (change_house_storage == 'Y') ? 480 : 440;
  }

/* The slots on the page at base: 40, or the 20 left on the last page. */
static Int32 page_slots(Int32 base) {
  return (500 - base < 40) ? 500 - base : 40;
  }

/* The first empty slot in the house, or -1 when it is full. */
static Int32 empty_slot(Int32 hnum) {
  Int32 i;

  for (i=0; i<500; i++)
    if (player.houses[hnum].inventory[i].otype == O_NONE) return i;
  return -1;
  }

/* The house's slots that show: up to the last page's */
static Int32 house_slots() {
  return last_base() + page_slots(last_base());
  }

  void house_show_inventory(Int32 base,Int32 hnum,OBJECT_REC* in_air,MENU* menu) {
    Int32 i;
    char buffer[80];
    textmode(3);
    textcolor(15); gotoxy(33,1); txt_write("House Inventory");
    if (improved_interface == 'Y') {
      draw_menu(menu);
      show_in_air(in_air,23);
      return;
      }
    sprintf(buffer,"     Page %d of %d",base/40+1,last_base()/40+1);
    txt_write(buffer);
    gotoxy(10,3);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("ESC");
    textcolor(6); txt_write(">Exit, <");
    textcolor(14); txt_write("ENTER");
    textcolor(6); txt_write(">Transfer Item, <");
    textcolor(14); txt_write("Arrows");
    textcolor(6); txt_write(">Select Item, <");
    textcolor(14); txt_write("I");
    textcolor(6); txt_write(">nfo");
    gotoxy(11,4);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("7");
    textcolor(6); txt_write("> Exchange up, <");
    textcolor(14); txt_write("1");
    textcolor(6); txt_write(">Exchange down, <");
    textcolor(14); txt_write("space");
    textcolor(6); txt_write("> Exchange In Air");
    textcolor(3);
    for (i=0; i<20; i++) {
      if (player.houses[hnum].inventory[i+base].otype != O_NONE) {
        gotoxy(3,i+5);
        sprintf(buffer,"%s",get_name(&(player.houses[hnum].inventory[i+base])));
        if (strlen(buffer) > 30) {
          buffer[30] = 0;
          strcat(buffer,"...");
          }
        itemColor(&(player.houses[hnum].inventory[i+base]));
        txt_write(buffer);
        }
      if (i+base+20 < 500 && player.houses[hnum].inventory[i+base+20].otype != O_NONE) {
        gotoxy(43,i+5);
        sprintf(buffer,"%s",get_name(&(player.houses[hnum].inventory[i+base+20])));
        if (strlen(buffer) > 30) {
          buffer[30] = 0;
          strcat(buffer,"...");
          }
        itemColor(&(player.houses[hnum].inventory[i+base+20]));
        txt_write(buffer);
        }
      }
    show_in_air(in_air,25);
    }

void house_inventory(Int32 hnum) {
  char flag;
  char key;
  Int32  row;
  Int32  col;
  Int32  base;
  Int32  i;
  OBJECT_REC  it;
  OBJECT_REC* item;
  OBJECT_REC  in_air;
  OBJECT_REC  temp;
  Int32 item1;
  Int32 item2;
  STORAGE     storage = { .slots = player.houses[hnum].inventory, .inair = &in_air };
  MENU        menu;

  row = 0;
  col = 0;
  base = 0;
  in_air.otype = O_NONE;
  open_storage(&menu,&storage,house_slots());
  house_show_inventory(base,hnum,&in_air,&menu);
  flag = ' ';
  while (flag == ' ') {
    if (improved_interface == 'Y') {
      key = storage_key(&menu);
      if (key == 13) key = storage_action(&menu,transfer_action(&(player.houses[hnum].inventory[menu.row])));
      if (key == STORAGE_MOVED) house_show_inventory(base,hnum,&in_air,&menu);
      base = menu.row / 40 * 40;
      col = menu.row % 40 / 20;
      row = menu.row % 20;
      }
    else {
      textcolor(14);
      gotoxy(1+col*40,row+5);
      txt_write("->");
      key = inkey();
      textcolor(14);
      gotoxy(1+col*40,row+5);
      txt_write("  ");
      }
    if (key == 27) flag = 'N';

    /* Without the House Storage change an item In Air is lost when you leave a full house */
    if (key == 27 && change_house_storage == 'Y' && in_air.otype != O_NONE && empty_slot(hnum) < 0) {
      textcolor(12);
      gotoxy(20,(improved_interface == 'Y') ? 24 : 2);
      txt_write("No room in the house for the item In Air");
      flag = ' ';
      }
    if (key == 13) {
      it = player.houses[hnum].inventory[base+row+col*20];
      player.houses[hnum].inventory[base+row+col*20].otype = O_NONE;
      if (it.otype != O_NONE) item = inventory(&it);
        else item = inventory(NULL);
      if (item != NULL &&
        player.houses[hnum].inventory[base+row+col*20].otype == O_NONE) {
        player.houses[hnum].inventory[base+row+col*20] = *item;
        item->otype = O_NONE;
        }
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == 'i' || key == 'I') {
      it = player.houses[hnum].inventory[base+row+col*20];
      if (it.otype != O_NONE) data_card(&it);
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == '3') {
      base += 40;
      if (base > last_base()) base = 0;
      if (col*20 >= page_slots(base)) col = 0;
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == '9') {
      base -= 40;
      if (base < 0) base = last_base();
      if (col*20 >= page_slots(base)) col = 0;
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == '2' || key == '.' || key == '>') {
      row++;
      if (row > 19) row = 0;
      }
    if (key == '8' || key == ',' || key == '<') {
      row--;
      if (row < 0) row = 19;
      }
    if (key == '6' || key == '4') {
      col = (col == 0) ? 1 : 0;
      if (col*20 >= page_slots(base)) col = 0;
      }
    if (key ==' ' || key == 'x' || key == 'X') {
      item1 = base+row+col*20;
      temp = player.houses[hnum].inventory[item1];
      player.houses[hnum].inventory[item1] = in_air;
      in_air = temp;
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == '7') {
      item1 = row+col*20;
      item2 = item1 - 1;
      if (item2 < 0) item2 = page_slots(base) - 1;
      temp = player.houses[hnum].inventory[base + item1];
      player.houses[hnum].inventory[base + item1] = player.houses[hnum].inventory[base + item2];
      player.houses[hnum].inventory[base + item2] = temp;
      col = item2 / 20;
      row = item2 - (col * 20);
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    if (key == '1') {
      item1 = row+col*20;
      item2 = item1 + 1;
      if (item2 > page_slots(base) - 1) item2 = 0;
      temp = player.houses[hnum].inventory[base + item1];
      player.houses[hnum].inventory[base + item1] = player.houses[hnum].inventory[base + item2];
      player.houses[hnum].inventory[base + item2] = temp;
      col = item2 / 20;
      row = item2 - (col * 20);
      house_show_inventory(base,hnum,&in_air,&menu);
      }
    }
  if (in_air.otype != O_NONE) {
    i = empty_slot(hnum);
    if (i >= 0) player.houses[hnum].inventory[i] = in_air;
    }
  textmode(3);
  }


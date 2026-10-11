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

static void show_spells(Int32 offset,Int32 page_size) {
  Int32 i;
  Int32 j;
  Int32 line,col;
  char buffer[80];
  char buffer2[80];
  Int32 count;
  line = 3;
  col = 12;
  textcolor(14);
  count = n_shop_items - offset;
  if (count > page_size) count = page_size;
  for (i=0; i<count; i++) {
    gotoxy(18,line);
    txt_write("                                                  ");
    sprintf(buffer,"%c)",i+'a');
    gotoxy(5,line); txt_write(buffer);
    strcpy(buffer2,shop_items[i+offset].name);
    for (j=0; j<strlen(buffer2); j++)
      if (buffer2[j] == '_') buffer2[j] = ' ';
    gotoxy(col,line);
    sprintf(buffer,"%3d",i+1+offset);
    txt_write(buffer);
    gotoxy(col+5,line);
    sprintf(buffer,"%s:%d",classes[spells[shop_items[i+offset].control].allign-1].name,spells[shop_items[i+offset].control].level);
    txt_write(buffer);
    gotoxy(col+20,line);
    txt_write(buffer2);
    gotoxy(col+50,line);
    sprintf(buffer,"%6.0f",buy_price(item_cost(&(shop_items[i+offset]))));
    txt_write(buffer);
    line++;
    }
  }

static void show_items(Int32 shop_type,Int32 offset,Int32 page_size) {
  Int32 count;
  Int32 i;
  Int32 j;
  Int32 line;
  char buffer[80];
  char buffer2[80];
  if (shop_type == SHOP_SPEL) {
    show_spells(offset,page_size);
    return;
    }
  line = 3;
  textcolor(14);
  count = n_shop_items - offset;
  if (count > page_size) count = page_size;
  for (i=0; i<count; i++) {
    gotoxy(18,line);
    txt_write("                                                  ");
    sprintf(buffer,"%c)",i+'a');
    gotoxy(11,line); txt_write(buffer);
    gotoxy(18,line);
    strcpy(buffer2,shop_items[i+offset].name);
    if (shop_items[i+offset].charges > 0) {
      sprintf(buffer," [%d]",shop_items[i+offset].charges);
      strcat(buffer2,buffer);
      }
    for (j=0; j<strlen(buffer2); j++)
      if (buffer2[j] == '_') buffer2[j] = ' ';
    sprintf(buffer,"%s",buffer2);
    txt_write(buffer);
    gotoxy(55,line);
    sprintf(buffer,"%6.0f",buy_price(item_cost(&(shop_items[i+offset]))));
    txt_write(buffer);
    line++;
    }
  }

static void show_top_spell(char* name,Int32 offset,Int32 page_size) {
  Int32 i;
  Int32 j;
  Int32 line,col;
  char buffer[80];
  char buffer2[80];
  Int32 count;
  line = 3;
  col = 12;
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);
  textcolor(14);
  count = n_shop_items - offset;
  if (count > page_size) count = page_size;
  for (i=0; i<count; i++) {
    strcpy(buffer2,shop_items[i+offset].name);
    for (j=0; j<strlen(buffer2); j++)
      if (buffer2[j] == '_') buffer2[j] = ' ';
    gotoxy(col,line);
    sprintf(buffer,"%3d",i+1+offset);
    txt_write(buffer);
    gotoxy(col+5,line);
    sprintf(buffer,"%s:%d",classes[spells[shop_items[i+offset].control].allign-1].name,spells[shop_items[i+offset].control].level);
    txt_write(buffer);
    gotoxy(col+20,line);
    txt_write(buffer2);
    gotoxy(col+50,line);
    sprintf(buffer,"%6.0f",buy_price(item_cost(&(shop_items[i+offset]))));
    txt_write(buffer);
    line++;
    }
  textcolor(14); gotoxy(35,25);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

static void show_top(char* name,Int32 shop_type,Int32 offset,Int32 page_size) {
  char buffer[80];
/*
  if (shop_type == SHOP_SPEL) {
    show_top_spell(name,offset,page_size);
    return;
    }
*/
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);
  show_items(shop_type,offset,page_size);
  textcolor(14); gotoxy(35,25);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

static void show_arrows(Int32 shop_type,Int32 line) {
  textcolor(14);
  if (shop_type == SHOP_SPEL) {
    gotoxy(8,line+3); txt_write("-->");
    gotoxy(69,line+3); txt_write("<--");
    } else {
    gotoxy(14,line+3); txt_write("-->");
    gotoxy(62,line+3); txt_write("<--");
    }
  }

static void erase_arrows(Int32 shop_type,Int32 line) {
  if (shop_type == SHOP_SPEL) {
    gotoxy(8,line+3); txt_write("   ");
    gotoxy(69,line+3); txt_write("   ");
    } else {
    gotoxy(14,line+3); txt_write("   ");
    gotoxy(62,line+3); txt_write("   ");
    }
  }

void shop(char* name,Int32 shop_type) {
  double price;
  OBJECT_REC item;
  Int32 offset;
  Int32 page_size;
  char buffer[80];
  char key;
  Int32  line;

  /* Without the Shop Boots and Wands change no shop asks for boots or wands, so those for sale are never stocked */
  switch (shop_type) {
    case SHOP_ALCH: get_shop_items(O_POTN,0,0,0,0,0,1000); break;
    case SHOP_ARMR: get_shop_items(O_ARMR,O_SHLD,O_HELM,(change_shop_boots_wands == 'Y') ? O_BOOT : 0,0,0,5000); break;
    case SHOP_BOOK: get_shop_items(O_SCRL,O_BOOK,0,0,0,0,1000); break;
    case SHOP_ROBE: get_shop_items(O_ROBE,0,0,0,0,0,1000); break;
    case SHOP_JEWL: get_shop_items(O_RING,O_NECK,O_JEWL,0,0,0,1000); break;
    case SHOP_MAGC: get_shop_items(O_STFF,O_MISC,(change_shop_boots_wands == 'Y') ? O_WAND : 0,0,0,0,500); break;
    case SHOP_WEAP: get_shop_items(O_EDGE,O_POLE,O_DAGR,O_BOW,O_BLNT,O_THRN,5000); break;
    case SHOP_FOOD: get_shop_items(O_FOOD,O_CONS,0,0,0,0,1000); break;
    case SHOP_CRFT: get_shop_items(O_CRFT,0,0,0,0,0,200); break;
    case SHOP_SPEL: get_shop_spells(99); break;
    case SHOP_RECP: get_shop_recipes(); break;
    }

  page_size = 18;
  offset = 0;
  show_top(name,shop_type,offset,page_size);
  gotoxy(15,22); textcolor(10);
  txt_write("Which item would you like (<ESC> to leave) ?");
  key = ' ';
  line = 0;
  show_arrows(shop_type,line);
  while (key != 27 && key != '0') {
    key = inkey();
    if (key =='2') {
      erase_arrows(shop_type,line);
      line++;
      if (offset+line >= n_shop_items) line--;
      if (line > 17) {
        line--;
        if (offset+18 < n_shop_items) {
          offset++;
          show_items(shop_type,offset,page_size);
          }
        }
      show_arrows(shop_type,line);
      }
    if (key =='8') {
      erase_arrows(shop_type,line);
      line--;
      if (line < 0) {
        line++;
        if (offset > 0) {
          offset--;
          show_items(shop_type,offset,page_size);
          }
        }
      show_arrows(shop_type,line);
      }
    if (key == '9') {
      erase_arrows(shop_type,line);
      offset -= 18;
      if (offset < 0) offset = 0;
      show_items(shop_type,offset,page_size);
      show_arrows(shop_type,line);
      }
    if (key == '3') {
      erase_arrows(shop_type,line);
      offset += 18;
      if (offset + 18 >= n_shop_items) {
        offset = n_shop_items - 18;
        if (offset < 0) offset = 0;
        }
      show_items(shop_type,offset,page_size);
      if (offset+line >= n_shop_items) line = n_shop_items - offset - 1;
      if (line < 0) line = 0;
      show_arrows(shop_type,line);
      }
    if (key == '7') {
      erase_arrows(shop_type,line);
      offset = 0;
      show_items(shop_type,offset,page_size);
      show_arrows(shop_type,line);
      }
    if (key == '1') {
      erase_arrows(shop_type,line);
      offset = 9999;
      if (offset + 18 >= n_shop_items) {
        offset = n_shop_items - 18;
        if (offset < 0) offset = 0;
        }
      show_items(shop_type,offset,page_size);
      if (offset+line >= n_shop_items) line = n_shop_items - offset - 1;
      if (line < 0) line = 0;
      show_arrows(shop_type,line);
      }
    if (key >= 'a' && key <= 'r') {
      erase_arrows(shop_type,line);
      line = key - 'a';
      if (offset+line >= n_shop_items) line = n_shop_items - offset - 1;
      show_arrows(shop_type,line);
      }
    if (key == 'I') {
      shop_items[offset+line].flags |= OF_IDENT;
      data_card(&(shop_items[offset + line]));
      show_top(name,shop_type,offset,page_size);
      show_arrows(shop_type,line);
      gotoxy(15,22); textcolor(10);
      txt_write("Which item would you like (<ESC> to leave) ?");
      }
    if (key == 13) {
      item = shop_items[offset+line];
      gotoxy(15,22); txt_write("                                                  ");
      gotoxy(15,22); textcolor(12);
      sprintf(buffer,"Ah, a %s A fine selection!",item.name);
      txt_write(buffer);
      price = buy_price(item_cost(&item));
      haggle(price,item);
      show_top(name,shop_type,offset,page_size);
      show_arrows(shop_type,line);
      gotoxy(15,22); textcolor(10);
      txt_write("Which item would you like (<ESC> to leave) ?");
      }
    }
  } /* void shop(char* name) */

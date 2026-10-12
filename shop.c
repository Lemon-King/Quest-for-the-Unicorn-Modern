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

#define SHOP_ROWS      18
#define SHOP_KEYS      "(8/2) Select   (ENTER) Buy   (I) Info   (ESC) Leave"
#define CARD_KEYS      "(8/2) Select   (ENTER) Buy   (ESC) Leave"
#define CARD_COLUMN    48
#define CARD_WIDTH     33

/* The item's name in the list, with its charges when they fit in width */
static void goods_name(Int32 i,Int32 width,char* name) {
  Int32 j;
  char buffer[16];

  strcpy(name,shop_items[i].name);
  sprintf(buffer," [%d]",shop_items[i].charges);
  if (shop_items[i].charges > 0 && (Int32)(strlen(name) + strlen(buffer)) <= width) strcat(name,buffer);
  for (j=0; j<strlen(name); j++)
    if (name[j] == '_') name[j] = ' ';
  }

static void goods_price(Int32 i,char* price) {
  sprintf(price,"%6.0f",buy_price(item_cost(&(shop_items[i]))));
  }

static void draw_spell(Int32 i,Int32 line) {
  Int32 j;
  Int32 col;
  char buffer[80];
  char name[80];

  col = 12;
  strcpy(name,shop_items[i].name);
  for (j=0; j<strlen(name); j++)
    if (name[j] == '_') name[j] = ' ';

  gotoxy(col,line);
  sprintf(buffer,"%3d",i+1);
  txt_write(buffer);

  gotoxy(col+5,line);
  sprintf(buffer,"%s:%d",classes[spells[shop_items[i].control].allign-1].name,spells[shop_items[i].control].level);
  txt_write(buffer);

  gotoxy(col+20,line);
  txt_write(name);

  gotoxy(col+50,line);
  goods_price(i,buffer);
  txt_write(buffer);
  }

static void draw_item(Int32 i,Int32 line) {
  char name[80];
  char price[16];

  goods_name(i,sizeof(name) - 1,name);
  goods_price(i,price);

  gotoxy(18,line);
  txt_write(name);

  gotoxy(55,line);
  txt_write(price);
  }

/* Item i's row, covering the row before it */
static void draw_goods(Int32 shop_type,Int32 i,Int32 line) {
  gotoxy(18,line);
  txt_write("                                                  ");

  if (shop_type == SHOP_SPEL) draw_spell(i,line);
    else draw_item(i,line);
  }

/* A page of the goods, each lettered for the key that selects it */
static void show_items(Int32 shop_type,Int32 offset,Int32 page_size) {
  Int32 count;
  Int32 i;
  char buffer[8];

  textcolor(14);
  count = n_shop_items - offset;
  if (count > page_size) count = page_size;

  for (i=0; i<count; i++) {
    draw_goods(shop_type,i+offset,i+3);
    sprintf(buffer,"%c)",i+'a');
    gotoxy((shop_type == SHOP_SPEL) ? 5 : 11,i+3);
    txt_write(buffer);
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

/* The shop's name over its goods and your gold below them */
static void show_frame(char* name) {
  char buffer[80];

  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  textcolor(14); gotoxy(35,25);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

static void show_top(char* name,Int32 shop_type,Int32 offset,Int32 page_size) {
/*
  if (shop_type == SHOP_SPEL) {
    show_top_spell(name,offset,page_size);
    return;
    }
*/
  show_frame(name);
  show_items(shop_type,offset,page_size);
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

static void ask_which(void) {
  gotoxy(15,22); textcolor(10);
  txt_write("Which item would you like (<ESC> to leave) ?");
  }

static void show_info(OBJECT_REC* item) {
  item->flags |= OF_IDENT;
  data_card(item);
  }

static void buy(OBJECT_REC item) {
  char buffer[80];

  gotoxy(15,22); txt_write("                                                  ");
  gotoxy(15,22); textcolor(12);
  sprintf(buffer,"Ah, a %s A fine selection!",item.name);
  txt_write(buffer);

  haggle(buy_price(item_cost(&item)),item);
  }

/* A row of the goods: a spell as without the preference, an item by its name and price */
static void draw_goods_row(MENU* menu,Int32 row,Int32 line) {
  char name[80];
  char price[16];

  if (*(Int32*)menu->screen == SHOP_SPEL) {
    textcolor(menu->color);
    draw_goods(SHOP_SPEL,row,line);
    return;
    }

  goods_name(row,menu_label_width(menu),name);
  goods_price(row,price);
  draw_menu_text(menu,line,name,price);
  }

/* The selected item's card beside the list, as (I) shows it */
static void show_card(MENU* menu) {
  OBJECT_REC item;
  char       blank[CARD_WIDTH + 1];
  Int32      line;

  sprintf(blank,"%*s",CARD_WIDTH,"");
  for (line=menu->rows.line + 1; line<=menu->rows.line + menu->rows.shown; line++) {
    gotoxy(CARD_COLUMN,line);
    txt_write(blank);
    }

  if (n_shop_items == 0) return;

  item = shop_items[menu->row];
  item.flags |= OF_IDENT;
  draw_item_card(&item,CARD_COLUMN,menu->rows.line + 1);
  }

static void show_menu_screen(char* name,MENU* menu) {
  show_frame(name);
  draw_menu(menu);
  ask_which();
  }

/* The shop with the Cursor Menus preference: the goods are one list, without letters. Items are listed on the left,
   with the selected one's card on the right; spells keep their wide rows and show their card with (I). */
static void shop_menu(char* name,Int32 shop_type) {
  MENU  menu = { .rows = { .shown = SHOP_ROWS, .line = 2 }, .color = 14, .keys = SHOP_KEYS, .keys_line = 24,
                 .draw_row = draw_goods_row, .screen = &shop_type };
  Int32 cards;
  char  key;

  cards = (shop_type != SHOP_SPEL);
  if (cards) {
    menu.column = 2;
    menu.value_column = 39;
    menu.rows.more_column = 39;
    menu.keys = CARD_KEYS;
    }
  else {
    menu.column = 8;
    menu.rows.more_column = 62;
    }

  fit_menu(&menu,n_shop_items);
  show_menu_screen(name,&menu);

  key = 0;
  while (key != 27 && key != '0') {
    if (cards) show_card(&menu);
    key = menu_key(&menu);
    if (n_shop_items == 0) continue;

    if (key == 'I' && !cards) {
      show_info(&(shop_items[menu.row]));
      show_menu_screen(name,&menu);
      }
    if (key == 13) {
      buy(shop_items[menu.row]);
      show_menu_screen(name,&menu);
      }
    }
  }

void shop(char* name,Int32 shop_type) {
  Int32 offset;
  Int32 page_size;
  char key;
  Int32  line;
  Int32  boots;
  Int32  wands;

  /* Without the Shop Boots and Wands change no shop stocks boots or wands */
  boots = (change_shop_boots_wands == 'Y') ? O_BOOT : 0;
  wands = (change_shop_boots_wands == 'Y') ? O_WAND : 0;

  switch (shop_type) {
    case SHOP_ALCH: get_shop_items(O_POTN,0,0,0,0,0,1000); break;
    case SHOP_ARMR: get_shop_items(O_ARMR,O_SHLD,O_HELM,boots,0,0,5000); break;
    case SHOP_BOOK: get_shop_items(O_SCRL,O_BOOK,0,0,0,0,1000); break;
    case SHOP_ROBE: get_shop_items(O_ROBE,0,0,0,0,0,1000); break;
    case SHOP_JEWL: get_shop_items(O_RING,O_NECK,O_JEWL,0,0,0,1000); break;
    case SHOP_MAGC: get_shop_items(O_STFF,O_MISC,wands,0,0,0,500); break;
    case SHOP_WEAP: get_shop_items(O_EDGE,O_POLE,O_DAGR,O_BOW,O_BLNT,O_THRN,5000); break;
    case SHOP_FOOD: get_shop_items(O_FOOD,O_CONS,0,0,0,0,1000); break;
    case SHOP_CRFT: get_shop_items(O_CRFT,0,0,0,0,0,200); break;
    case SHOP_SPEL: get_shop_spells(99); break;
    case SHOP_RECP: get_shop_recipes(); break;
    }

  /* With the Village Shops change a village's weapons and armour are only levels 1 and 2 */
  if (change_village_shops == 'Y' && in_village() == 'Y' && (shop_type == SHOP_WEAP || shop_type == SHOP_ARMR))
    limit_shop_items(2);

  if (cursor_menus == 'Y') {
    shop_menu(name,shop_type);
    return;
    }

  page_size = SHOP_ROWS;
  offset = 0;
  show_top(name,shop_type,offset,page_size);
  ask_which();
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
      show_info(&(shop_items[offset+line]));
      show_top(name,shop_type,offset,page_size);
      show_arrows(shop_type,line);
      ask_which();
      }
    if (key == 13) {
      buy(shop_items[offset+line]);
      show_top(name,shop_type,offset,page_size);
      show_arrows(shop_type,line);
      ask_which();
      }
    }
  } /* void shop(char* name) */

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

#define NUM_TRADES      4

/* Picked by 11 to 14 in the numbered menu, after the items for sale */
static char* trades[NUM_TRADES] = { "Sell an item", "Identify an item", "Sell from Pack", "Identify from Pack" };

/* The Mech-Merchant's menu: items[0] to items[count-1] for sale, then the trades */
typedef struct {
  OBJECT_REC* items;
  Int32       count;
  } MECH_MENU;

static double mech_price(OBJECT_REC* item) {
  return item_cost(item) * 1.50;
  }

static void draw_mech_row(MENU* menu,Int32 row,Int32 line) {
  MECH_MENU* mech;
  char       price[16];

  mech = menu->screen;
  if (row < mech->count) {
    sprintf(price,"%6.0f",mech_price(&(mech->items[row])));
    draw_menu_named(menu,line,get_name(&(mech->items[row])),item_color(&(mech->items[row])),price);
    }
  else draw_menu_text(menu,line,trades[row - mech->count],"");
  }

/* The number that picks the chosen row in the numbered menu, or 0 to leave */
static Int32 pick_option(MENU* menu) {
  MECH_MENU* mech;
  Int32      row;

  mech = menu->screen;
  row = menu_choice(menu);

  if (row < 0) return 0;
  if (row < mech->count) return row + 1;
  return 11 + row - mech->count;
  }

static void show_top(OBJECT_REC* items,MENU* menu) {
  Int32 i;
  Int32 pos;
  char buffer[80];
  char buffer2[80];

  textmode(3); gotoxy(33,1); textcolor(11);
  sprintf(buffer,"Mech-Merchant");
  txt_write(buffer);

  textcolor(14);
  if (improved_interface == 'Y') draw_menu(menu);
  else {
    pos = 1;
    for (i=0; i<10; i++) {
      if (items[i].otype != O_NONE) {
        gotoxy(22,pos+2);
        strcpy(buffer2, get_name(&(items[i])));
        sprintf(buffer,"%2d  ",pos);
        write_named(buffer,&(items[i]),buffer2,"",14);
        gotoxy(50,pos+2);
        sprintf(buffer,"%6.0f",mech_price(&(items[i])));
        txt_write(buffer);
        pos++;
        }
      }
    for (i=0; i<NUM_TRADES; i++) {
      sprintf(buffer,"%2d  %s",11+i,trades[i]);
      gotoxy(22,14+i); txt_write(buffer);
      }
    }

  show_gold(35,24);
  }

static double sell_price(OBJECT_REC* item) {
  double price;
  price = item_cost(item);
  if (item->otype == O_JEWL) {
    price *= 0.95;
    price *= 0.70;
    return price;
    }
  price *= 0.08;
  if ((item->flags & OF_IDENT) == 0) price /= 2.0;
  price *= 0.70;
  return price;
  }

/* The merchant's offer for an item: at least 1, in the whole gold add_gold() pays */
static double offer(OBJECT_REC* item) {
  double price;

  price = sell_price(item);
  if (price < 1) price = 1;

  return rcs_trunc(price);
  }

static double identify_price(OBJECT_REC* item) {
  return rcs_trunc(item_cost(item) * 0.25);
  }

void mech_merchant() {
  Int32 option;
  double price;
  OBJECT_REC item;
  OBJECT_REC* pItem;
  Int32 j,i;
  char buffer[80];
  char key;
  OBJECT_REC items[10];
  MECH_MENU mech = { .items = items };
  MENU menu = { .rows = { .shown = 10 + NUM_TRADES, .line = 2 }, .column = 22, .value_column = 50, .color = 14,
                .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_mech_row, .screen = &mech };

  i = 0;
  pItem = find_linked_item(40); if (pItem != NULL) items[i++] = *pItem;
  pItem = find_linked_item(499); if (pItem != NULL) items[i++] = *pItem;
  mech.count = i;
  fit_menu(&menu,mech.count + NUM_TRADES);
  while (i < 10) items[i++].otype = O_NONE;
  pItem = &item;
  show_top(items,&menu);
  option = 1;
  while (option != 0) {
    gotoxy(15,19); textcolor(10);
    if (improved_interface == 'Y') {
      txt_write("Which item would you like (<ESC> to leave)?");
      option = pick_option(&menu);
      }
    else {
      txt_write("Which item would you like (<ESC> to leave) ?        ");
      gotoxy(60,19); textcolor(15);
      strcpy(buffer,"");
      if (p_read(buffer,sizeof(buffer)) == 0) {
        sscanf(buffer,"%d",&option);
        } else option = 0;
      }
    for (i=18; i<=23; i++) {
      gotoxy(10,i); txt_write("                                                                    ");
      }
    if (option>0 && option<11 && items[option-1].otype != O_NONE) {
      item = items[option-1];
      gotoxy(15,19);
      if (improved_interface == 'Y') {
        list_item_name(&item,60,buffer);
        write_named("Ah, a ",&item,buffer," A fine selection!",12);
        }
      else {
        textcolor(12);
        sprintf(buffer,"Ah, a %s A fine selection!",item.name);
        txt_write(buffer);
        }
      price = mech_price(&item);
      if (price <= get_gold()) {
        inventory(&item);
        sub_gold(price);
        }
      else {
        gotoxy(15,21);
        txt_write("You do not have that much!! (press a key)");
        key = inkey();
        }
      show_top(items,&menu);
      }
    if (option == 12) {
      pItem = inventory(0);
      show_top(items,&menu);
      if (pItem != NULL && pItem->otype != O_NONE) {
        gotoxy(15,19);
        price = identify_price(pItem);
        write_named("To identify a ",pItem,get_name(pItem),"",12);
        gotoxy(15,20); textcolor(14);
        sprintf(buffer," will cost %.0f Gold",price);
        txt_write(buffer);
        if (price>get_gold()) {
          gotoxy(15,21);
          txt_write("You do not have that much!! (press a key)");
          key = inkey();
          }
        else {
          if (improved_interface == 'Y') key = ask_action(15,21,10,"Identify","Cancel",0);
          else {
            gotoxy(15,21); textcolor(10);
            txt_write("Do you wish to identify it ? ");
            key = inkey();
            }
          if (key == 'y' || key == 'Y') {
            identify_item(pItem,price);
            gotoxy(15,22);
            write_named("It is a ",pItem,get_name(pItem)," (press a key)",15);
            key = inkey();
            }
          }
        }
      show_top(items,&menu);
      }
    if (option == 13 && improved_interface == 'Y') {
      sell_from_pack(offer);
      show_top(items,&menu);
      }
    else if (option == 13)
      for (i=1; i<=32; i++)
        if (player.pack[i].otype != O_NONE) {
          price = offer(&(player.pack[i]));
          textcolor(11); gotoxy(22,19);
          sprintf(buffer,"You have ");
          txt_write(buffer);
          itemColor(&(player.pack[i]));
          sprintf(buffer,"%s",get_name(&(player.pack[i])));
          txt_write(buffer);
          textcolor(11);
          gotoxy(22,20);
          sprintf(buffer,"I offer you %.0f",price);
          txt_write(buffer);
          if (improved_interface == 'Y') key = ask_action(22,21,15,"Sell","Keep",1);
          else {
            gotoxy(22,21); textcolor(15);
            txt_write("Do you wish to sell ?");
            key = inkey();
            while (key != 'Y' && key != 'y' &&
                   key != 'N' && key != 'n') key = inkey();
            }
          if (key == 'Y' || key == 'y') sell_item(&(player.pack[i]),price);
          show_gold(35,24);
          for (j=18; j<=23; j++) {
            gotoxy(10,j); txt_write("                                                                    ");
            }
          option = 13;
        }
    if (option == 14 && improved_interface == 'Y') {
      identify_from_pack(identify_price);
      show_top(items,&menu);
      }
    else if (option == 14)
      for (i=1; i<=32; i++)
        if (player.pack[i].otype != O_NONE && (player.pack[i].flags & OF_IDENT) == 0) {
          gotoxy(15,19);
          price = identify_price(&(player.pack[i]));
          write_named("To identify a ",&(player.pack[i]),get_name(&(player.pack[i])),"",12);
          gotoxy(15,20); textcolor(14);
          sprintf(buffer," will cost %.0f Gold",price);
          txt_write(buffer);
          if (price>get_gold()) {
            gotoxy(15,21);
            txt_write("You do not have that much!! (press a key)");
            key=inkey();
            }
          else {
            if (improved_interface == 'Y') key = ask_action(15,21,10,"Identify","Cancel",0);
            else {
              gotoxy(15,21); textcolor(10);
              txt_write("Do you wish to identify it ? ");
              key = inkey();
              }
            if (key == 'y' || key == 'Y') {
              identify_item(&(player.pack[i]),price);
              gotoxy(15,22);
              write_named("It is a ",&(player.pack[i]),get_name(&(player.pack[i]))," (press a key)",15);
              key = inkey();
              }
            }
          show_gold(35,24);
          for (j=18; j<=23; j++) {
            gotoxy(10,j); txt_write("                                                                    ");
            }
          option = 14;
        }
    if (option == 11) {
      pItem = inventory(0);
      show_top(items,&menu);
      if (pItem != NULL && pItem->otype != O_NONE) {
        price = offer(pItem);
        gotoxy(15,19);
        write_named("Ah, a ",pItem,get_name(pItem),"",12);
        gotoxy(15,21); textcolor(9);
        switch (u_random(5)) {
          case 0:txt_write("Its pretty dirty, I will give you ");
                 break;
          case 1:txt_write("It looks well used, I will give you ");
                 break;
          case 2:txt_write("It look pretty old, I will give you ");
                 break;
          case 3:txt_write("It looks pretty battered, I will give you ");
                 break;
          case 4:txt_write("There is not much market for that, I will give you ");
                 break;
          }
        sprintf(buffer,"%.0f",price);
        txt_write(buffer);
        if (improved_interface == 'Y') key = ask_action(15,22,9,"Sell","Keep",0);
        else {
          gotoxy(15,22); txt_write("Do you wish to sell it ?");
          key = inkey();
          }
        if (key == 'Y' || key == 'y') sell_item(pItem,price);
        }
      show_top(items,&menu);
      }
    }
  } /* void pawn(char* name, byte city) */

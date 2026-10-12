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

/* The pawn shop's menu: the items for sale, in slots[0] to slots[count-1] of the city's pawn, then the trades */
typedef struct {
  byte   city;
  Int32  slots[10];
  Int32  count;
  } PAWN_MENU;

static void pawn_name(OBJECT_REC* item,char* name) {
  Int32 j;

  strcpy(name,item->name);
  for (j=0; j<strlen(name); j++)
    if (name[j] == '_') name[j] = ' ';
  }

static void draw_pawn_row(MENU* menu,Int32 row,Int32 line) {
  PAWN_MENU*  pawn;
  OBJECT_REC* item;
  char        name[80];
  char        price[16];

  pawn = menu->screen;
  if (row < pawn->count) {
    item = &(player.pawn[pawn->city][pawn->slots[row]]);
    pawn_name(item,name);
    sprintf(price,"%6.0f",buy_price(item_cost(item)));
    draw_menu_text(menu,line,name,price);
    }
  else draw_menu_text(menu,line,trades[row - pawn->count],"");
  }

/* Lists the items for sale, which change as they are bought, then draws the menu */
static void show_menu(MENU* menu) {
  PAWN_MENU* pawn;
  Int32      i;

  pawn = menu->screen;
  pawn->count = 0;
  for (i=1; i<=10; i++)
    if (player.pawn[pawn->city][i].otype != O_NONE) pawn->slots[pawn->count++] = i;

  fit_menu(menu,pawn->count + NUM_TRADES);
  draw_menu(menu);
  }

/* The number that picks the chosen row in the numbered menu, or 0 to leave */
static Int32 pick_option(MENU* menu) {
  PAWN_MENU* pawn;
  Int32      row;

  pawn = menu->screen;
  row = menu_choice(menu);

  if (row < 0) return 0;
  if (row < pawn->count) return pawn->slots[row];
  return 11 + row - pawn->count;
  }

static void show_top(char* name,byte city,MENU* menu) {
  Int32 i;
  Int32 pos;
  char buffer[80];
  char buffer2[80];

  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  textcolor(14);
  if (cursor_menus == 'Y') show_menu(menu);
  else {
    pos = 1;
    for (i=1; i<=10; i++) {
      if (player.pawn[city][i].otype != O_NONE) {
        gotoxy(22,pos+2);
        pawn_name(&(player.pawn[city][i]),buffer2);
        sprintf(buffer,"%2d  %s",pos,buffer2);
        txt_write(buffer);
        gotoxy(50,i+2);
        sprintf(buffer,"%6.0f",buy_price(item_cost(&(player.pawn[city][i]))));
        txt_write(buffer);
        pos++;
        }
      }
    for (i=0; i<NUM_TRADES; i++) {
      sprintf(buffer,"%2d  %s",11+i,trades[i]);
      gotoxy(22,14+i); txt_write(buffer);
      }
    }

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

static double sell_price(OBJECT_REC* item) {
  double price;
  double discount;
  price = item_cost(item);
  if (item->otype == O_JEWL) {
    price *= 0.95;
    return price;
    }
  price *= 0.08;
  if ((item->flags & OF_IDENT) == 0) price /= 2.0;
  discount = 30.0 * (get_skill(SK_CHARM)/100.0);
  discount = 1.0 + (discount / 100.0);
  if (player.sex == 'F') discount += 0.02;
  if (player.stat[6] >= 0 && player.stat[6] <= 7) discount -= .16;
  if (player.stat[6] >= 8 && player.stat[6] <= 11) discount -= .10;
  if (player.stat[6] >= 12 && player.stat[6] <= 14) discount -= .06;
  if (player.stat[6] >= 17 && player.stat[6] <= 18) discount += .02;
  if (player.stat[6] >= 19 && player.stat[6] <= 20) discount += .04;
  if (player.stat[6] >= 21 && player.stat[6] <= 23) discount += .06;
  if (player.stat[6] >= 24 && player.stat[6] <= 99) discount += .10;
  if (classes[player.class-1].flags & CF_NOBLE) discount += 0.15;
  price = price * discount;
  return price;
  }

void pawn(char* name, byte city) {
  Int32 option;
  double price;
  OBJECT_REC item;
  OBJECT_REC* pItem;
  Int32 j,i;
  char buffer[80];
  char key;
  PAWN_MENU shop = { .city = city };
  MENU menu = { .rows = { .shown = 10 + NUM_TRADES, .line = 2 }, .column = 22, .value_column = 50, .color = 14,
                .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_pawn_row, .screen = &shop };

  pItem = &item;
  show_top(name,city,&menu);
  option = 1;
  while (option != 0) {
    gotoxy(15,19); textcolor(10);
    if (cursor_menus == 'Y') {
      txt_write("Which item would you like (<ESC> to leave) ?");
      option = pick_option(&menu);
      }
    else {
      txt_write("Which item would you like (<ESC> to leave) ?        ");
      gotoxy(60,19); textcolor(15);
      strcpy(buffer,"");
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%d",&option);
        } else option = 0;
      }
    for (i=18; i<=23; i++) {
      gotoxy(10,i); txt_write("                                                                    ");
      }
    if (option>0 && option<11 && player.pawn[city][option].otype != O_NONE) {
      item = player.pawn[city][option];
      pawn_name(&item,buffer);
      gotoxy(15,19);
      write_named("Ah, a ",&item,buffer," A fine selection!",12);
      price = buy_price(item_cost(&item));
      if (haggle(price,item)) {
        for (i=option; i<10; i++) player.pawn[city][i] = player.pawn[city][i+1];
        player.pawn[city][10].otype = O_NONE;
        }
      show_top(name,city,&menu);
      }
    if (option == 12) {
      pItem = inventory(0);
      show_top(name,city,&menu);
      if (pItem != NULL && pItem->otype != O_NONE) {
        gotoxy(15,19);
        price = rcs_trunc(item_cost(pItem) * 0.08);
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
          if (cursor_menus == 'Y') key = ask_action(15,21,10,"Identify","Cancel",0);
          else {
            gotoxy(15,21); textcolor(10);
            txt_write("Do you wish to identify it ? ");
            key = inkey();
            }
          if (key == 'y' || key == 'Y') {
            sub_gold(price);
            pItem->flags |= OF_IDENT;
            if (pItem->effect == E_MUTA) mutable_set_name(pItem);
            gotoxy(15,22);
            write_named("It is a ",pItem,get_name(pItem)," (press a key)",15);
            key = inkey();
            }
          }
        }
      show_top(name,city,&menu);
      }
    if (option == 13)
      for (i=1; i<=32; i++)
        if (player.pack[i].otype != O_NONE) {
          price = sell_price(&(player.pack[i]));
	  if (price < 1) price = 1;
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
          if (cursor_menus == 'Y') key = ask_action(22,21,15,"Sell","Keep",1);
          else {
            gotoxy(22,21); textcolor(15);
            txt_write("Do you wish to sell ?");
            key = inkey();
            while (key != 'Y' && key != 'y' &&
                   key != 'N' && key != 'n') key = inkey();
            }
          if (key == 'Y' || key == 'y') {
            player.pack[i].otype = O_NONE;
            add_gold(price);
            stats.counts[STAT_ITEMS_SOLD]++;
            stats.counts[STAT_SOLD_GOLD] += price;
            if (price > stats.counts[STAT_LARGEST_SALE]) stats.counts[STAT_LARGEST_SALE] = price;
            }
          textcolor(14); gotoxy(35,24);
          sprintf(buffer,"Gold = %d",get_gold());
          txt_write(buffer);
          for (j=18; j<=23; j++) {
            gotoxy(10,j); txt_write("                                                                    ");
            }
          option = 13;
        }
    if (option == 14)
      for (i=1; i<=32; i++)
        if (player.pack[i].otype != O_NONE && (player.pack[i].flags & OF_IDENT) == 0) {
          gotoxy(15,19);
          price = rcs_trunc(item_cost(&(player.pack[i])) * 0.08);
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
            if (cursor_menus == 'Y') key = ask_action(15,21,10,"Identify","Cancel",0);
            else {
              gotoxy(15,21); textcolor(10);
              txt_write("Do you wish to identify it ? ");
              key = inkey();
              }
            if (key == 'y' || key == 'Y') {
              sub_gold(price);
              player.pack[i].flags |= OF_IDENT;
              if (player.pack[i].effect == E_MUTA) mutable_set_name(&(player.pack[i]));
              gotoxy(15,22);
              write_named("It is a ",&(player.pack[i]),get_name(&(player.pack[i]))," (press a key)",15);
              key = inkey();
              }
            }
          textcolor(14); gotoxy(35,24);
          sprintf(buffer,"Gold = %d",get_gold());
          txt_write(buffer);
          for (j=18; j<=23; j++) {
            gotoxy(10,j); txt_write("                                                                    ");
            }
          option = 14;
        }
    if (option == 11) {
      pItem = inventory(0);
      show_top(name,city,&menu);
      if (pItem != NULL && pItem->otype != O_NONE) {
        price = sell_price(pItem);
	if (price < 1) price = 1;
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
        if (cursor_menus == 'Y') key = ask_action(15,22,9,"Sell","Keep",0);
        else {
          gotoxy(15,22); txt_write("Do you wish to sell it ?");
          key = inkey();
          }
        if (key == 'Y' || key == 'y') {
          add_gold(price);
          pItem->otype = O_NONE;
          stats.counts[STAT_ITEMS_SOLD]++;
          stats.counts[STAT_SOLD_GOLD] += price;
          if (price > stats.counts[STAT_LARGEST_SALE]) stats.counts[STAT_LARGEST_SALE] = price;
          }
        }
      show_top(name,city,&menu);
      }
    }
  } /* void pawn(char* name, byte city) */

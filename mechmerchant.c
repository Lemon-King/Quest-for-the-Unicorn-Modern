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

void mech_merchant() {
  Int32 option;
  double price;
  OBJECT_REC item;
  OBJECT_REC* pItem;
  Int32 j,i;
  char buffer[80];
  char key;
  OBJECT_REC items[10];

  void show_top() {
    Int32 pos;
    char buffer2[80];
    textmode(3); gotoxy(33,1); textcolor(11);
    sprintf(buffer,"Mech-Merchant");
    txt_write(buffer);
    textcolor(14);
    pos = 1;
    for (i=0; i<10; i++) {
      if (items[i].otype != O_NONE) {
        gotoxy(22,pos+2);
        strcpy(buffer2, get_name(&(items[i])));
        sprintf(buffer,"%2d  %s",pos,buffer2);
        txt_write(buffer);
        gotoxy(50,pos+2);
        sprintf(buffer,"%6.0f",item_cost(&(items[i])) * 1.50);
        txt_write(buffer);
        pos++;
        }
      }
    gotoxy(22,14); txt_write("11  Sell an item");
    gotoxy(22,15); txt_write("12  Identify an item");
    gotoxy(22,16); txt_write("13  Sell from Pack");
    gotoxy(22,17); txt_write("14  Identify from Pack");
    textcolor(14); gotoxy(35,24);
    sprintf(buffer,"Gold = %d",get_gold());
    txt_write(buffer);
  }

  double sell_price(OBJECT_REC* item) {
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

  i = 0;
  pItem = find_linked_item(40); if (pItem != NULL) items[i++] = *pItem;
  pItem = find_linked_item(499); if (pItem != NULL) items[i++] = *pItem;
  while (i < 10) items[i++].otype = O_NONE;
  pItem = &item;
  show_top();
  option = 1;
  while (option != 0) {
    gotoxy(15,19); textcolor(10);
    txt_write("Which item would you like (<ESC> to leave) ?        ");
    gotoxy(60,19); textcolor(15);
    strcpy(buffer,"");
    if (p_read(buffer) == 0) {
      sscanf(buffer,"%d",&option);
      } else option = 0;
    for (i=18; i<=23; i++) {
      gotoxy(10,i); txt_write("                                                                    ");
      }
    if (option>0 && option<11 && items[option-1].otype != O_NONE) {
      item = items[option-1];
      gotoxy(15,19); textcolor(12);
      sprintf(buffer,"Ah, a %s A fine selection!",item.name);
      txt_write(buffer);
      price = item_cost(&item) * 1.50;
      if (price <= get_gold()) {
        inventory(&item);
        sub_gold(price);
        }
      else {
        gotoxy(15,21);
        txt_write("You do not have that much!! (press a key)");
        key = inkey();
        }
      show_top();
      }
    if (option == 12) {
      pItem = inventory(0);
      show_top();
      if (pItem != NULL && pItem->otype != O_NONE) {
        gotoxy(15,19); textcolor(12);
        price = rcs_trunc(item_cost(pItem) * 0.25);
        sprintf(buffer,"To identify a %s",get_name(pItem));
        txt_write(buffer);
        gotoxy(15,20); textcolor(14);
        sprintf(buffer," will cost %.0f Gold",price);
        txt_write(buffer);
        if (price>get_gold()) {
          gotoxy(15,21);
          txt_write("You do not have that much!! (press a key)");
          key = inkey();
          }
        else {
          gotoxy(15,21); textcolor(10);
          txt_write("Do you wish to identify it ? ");
          key = inkey();
          if (key == 'y' || key == 'Y') {
            sub_gold(price);
            pItem->flags |= OF_IDENT;
            if (pItem->effect == E_MUTA) mutable_set_name(pItem);
            gotoxy(15,22); textcolor(15);
            sprintf(buffer,"It is a %s (press a key)",get_name(pItem));
            txt_write(buffer);
            key = inkey();
            }
          }
        }
      show_top();
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
          gotoxy(22,21); textcolor(15);
          txt_write("Do you wish to sell ?");
          key = inkey();
          while (key != 'Y' && key != 'y' &&
                 key != 'N' && key != 'n') key = inkey();
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
          gotoxy(15,19); textcolor(12);
          price = rcs_trunc(item_cost(&(player.pack[i])) * 0.25);
          sprintf(buffer,"To identify a %s",get_name(&(player.pack[i])));
          txt_write(buffer);
          gotoxy(15,20); textcolor(14);
          sprintf(buffer," will cost %.0f Gold",price);
          txt_write(buffer);
          if (price>get_gold()) {
            gotoxy(15,21);
            txt_write("You do not have that much!! (press a key)");
            key=inkey();
            }
          else {
            gotoxy(15,21); textcolor(10);
            txt_write("Do you wish to identify it ? ");
            key = inkey();
            if (key == 'y' || key == 'Y') {
              sub_gold(price);
              player.pack[i].flags |= OF_IDENT;
              if (player.pack[i].effect == E_MUTA) mutable_set_name(&(player.pack[i]));
              gotoxy(15,22); textcolor(15);
              sprintf(buffer,"It is a %s (press a key)",get_name(&(player.pack[i])));
              txt_write(buffer);
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
      show_top();
      if (pItem != NULL && pItem->otype != O_NONE) {
        price = sell_price(pItem);
	if (price < 1) price = 1;
        gotoxy(15,19); textcolor(12);
        sprintf(buffer,"Ah, a %s",get_name(pItem));
        txt_write(buffer);
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
        gotoxy(15,22); txt_write("Do you wish to sell it ?");
        key = inkey();
        if (key == 'Y' || key == 'y') {
          add_gold(price);
          pItem->otype = O_NONE;
          stats.counts[STAT_ITEMS_SOLD]++;
          stats.counts[STAT_SOLD_GOLD] += price;
          if (price > stats.counts[STAT_LARGEST_SALE]) stats.counts[STAT_LARGEST_SALE] = price;
          }
        }
      show_top();
      }
    }
  } /* void pawn(char* name, byte city) */

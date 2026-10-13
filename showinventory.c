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

/* Cuts a name longer than width to width, then adds "..." */
void shorten_name(char* name,Int32 width) {
  if ((Int32)strlen(name) > width) {
    name[width] = 0;
    strcat(name,"...");
    }
  }

/* A name for a list, without its trailing spaces, cut to width with its "..." */
static void fit_list_name(char* name,Int32 width) {
  Int32 end;

  end = (Int32)strlen(name);
  while (end > 0 && name[end - 1] == ' ') end--;
  name[end] = 0;

  shorten_name(name,width - 3);
  }

/* An item's name for a list, cut to width with its "...", or blank for no item */
void list_item_name(OBJECT_REC* item,Int32 width,char* name) {
  strcpy(name,"");
  if (item->otype != O_NONE) strcpy(name,get_name(item));

  fit_list_name(name,width);
  }

/* An item you carry or store, named for a list as list_item_name() names it, with its * (marked_item_name()) */
void kept_item_name(OBJECT_REC* item,Int32 width,char* name) {
  strcpy(name,"");
  if (item->otype != O_NONE) marked_item_name(item,name);

  fit_list_name(name,width);
  }

void show_inventory(OBJECT_REC* item) {
  Int32 i;
  char buffer[256];
  textcolor(3);
    gotoxy(25,3);
    if (item != NULL && item->otype != O_NONE) {
      itemColor(item);
      txt_write("                                                ");
      gotoxy(25,3);
      marked_item_name(item,buffer);
      shorten_name(buffer,40);
      txt_write(buffer);
      } 
    else txt_write("                                                ");
    for (i=4; i<=22; i++) {
      gotoxy(25,i);
      if (player.invent[i-3].otype != O_NONE) {
        itemColor(&(player.invent[i-3]));
        txt_write("                                                ");
        gotoxy(25,i);
        marked_item_name(&(player.invent[i-3]),buffer);
        shorten_name(buffer,40);
        txt_write(buffer);
//        txt_write(get_name(&(player.invent[i-3])));
        }
      else txt_write("                                                ");
      }
  }


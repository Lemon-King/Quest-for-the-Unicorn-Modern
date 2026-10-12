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

#define NUM_MOUNTS      6

/* The mounts for sale, picked by '1' to '6' in the numbered menu: each one's name, monster link and price */
typedef struct {
  char*  name;
  Int32  link;
  Int32  cost;
  } MOUNT_REC;

static MOUNT_REC mounts_for_sale[NUM_MOUNTS] = {
  { "Horse",           101,   1000 },
  { "Centaur",         102,  75000 },
  { "Pegasus",          20, 125000 },
  { "Nightmare",       103, 160000 },
  { "Green Dragon",     62, 450000 },
  { "Platinum Dragon",  34, 750000 }
  };

static void draw_mount(MENU* menu,Int32 row,Int32 line) {
  char price[16];

  sprintf(price,"%6d",mounts_for_sale[row].cost);
  draw_menu_text(menu,line,mounts_for_sale[row].name,price);
  }

static void show_top(char* name,MENU* menu) {
  Int32 i;
  char buffer[80];

  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  textcolor(14);
  if (cursor_menus == 'Y') draw_menu(menu);
  else {
    for (i=0; i<NUM_MOUNTS; i++) {
      sprintf(buffer,"%d> %s",i+1,mounts_for_sale[i].name);
      gotoxy(27,i+3); txt_write(buffer);
      sprintf(buffer,"%6d",mounts_for_sale[i].cost);
      gotoxy(51,i+3); txt_write(buffer);
      }
    gotoxy(27,10); txt_write("0> Leave");
    gotoxy(30,12); txt_write("Option ?");
    }

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

void mounts(char* name) {
  double cost = 0;
  Int32 item = 0;
  Int32 slot;
  Int32 i,j;
  char key =' ';
  MENU menu = { .rows = { .count = NUM_MOUNTS, .shown = NUM_MOUNTS, .line = 2 }, .column = 26, .value_column = 51,
                .color = 14, .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_mount };

  show_top(name,&menu);
  while (key != '0') {
    if (cursor_menus == 'Y') {
      key = menu_number(&menu);
      if (key == 27) key = '0';
      }
    else {
      gotoxy(30,12); textcolor(14); txt_write("Option ?");
      key = inkey();
      }
    if (key>='1' && key<='6') {
      item = mounts_for_sale[key - '1'].link;
      cost = mounts_for_sale[key - '1'].cost;
      if (cost>get_gold()) {
        gotoxy(30,14); textcolor(4);
        txt_write("You do not have enough gold!!!!");
        delay(1500);
        }
      else {
       slot = 0;
       for (i=1; i<=5; i++)
         if (slot == 0 && player.party[i].hp == 0) slot = i;
       if (slot != 0) {
         j = (player.effect[PE_HEROIC] > 0) ? player.effect[PE_HEROIC] : 1;
         player.party[slot] = monsters[get_linked_monster(item)];
         player.party[slot].hp = player.party[slot].level * (9 * j);
         player.party[slot].time = -1;
         player.party[slot].exp = needed(player.party[slot].level);

         /* Without the Platinum Dragon Mount change the Platinum Dragon (34) keeps the wild monster's flags */
         if (change_platinum_dragon_mount == 'Y' && item == 34)
           player.party[slot].special |= MS_MOUNT | MS_FLY;
         sub_gold(cost);
         gotoxy(30,14); textcolor(11);
         txt_write("Mount Obtained.");
         delay(1500);
         }
       else {
         gotoxy(30,14); textcolor(11);
         txt_write("No room in party for mount.");
         delay(1500);
         }
       }
      }
      show_top(name,&menu);
    }
  check_mount();
  }

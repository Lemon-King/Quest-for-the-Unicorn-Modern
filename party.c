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

#define PARTY_KEYS "<8/2>Select, <ENTER>Statistics, <D>ismiss, <C>hange order, <O>rders, <Q>uit"
#define NUM_ORDERS 2

/* Picked by 'a' and 'e' without the Improved Interface preference */
static char* order_names[NUM_ORDERS] = { "Attack", "Evade" };

static void draw_member(MENU* menu,Int32 row,Int32 line) {
  NPC_REC* member;
  char     label[48];
  char     value[16];

  member = &(player.party[row + 1]);
  sprintf(label,"%2d: %s",row + 1,(member->hp != 0) ? member->name : "");
  if (member->hp != 0) sprintf(value,"%2d    %3d",member->level,member->hp);
    else sprintf(value,"%9s","");

  draw_menu_text(menu,line,label,value);
  }

static void party_show_top(MENU* menu) {
  Int32  i;
  char buffer[80];

  gotoxy(24,1); textcolor(2);
  txt_write("  #  Name               Lvl     Hp");

  if (improved_interface == 'Y') draw_menu(menu);
  else {
    for (i=1; i<=5; i++) {
      gotoxy(25,i + 2); textcolor(14);
      sprintf(buffer,"%2d: ",i);
      txt_write(buffer);
      if (player.party[i].hp != 0) {
        txt_write(player.party[i].name);
        gotoxy(49,i+2);
        sprintf(buffer,"%2d    %3d",player.party[i].level,player.party[i].hp);
        txt_write(buffer);
        }
      else {
        txt_write("                                                  ");
        }
      }
    }

  gotoxy(40,9); textcolor(14);
  txt_write("P");
  gotoxy(30,10); if (player.party[1].hp != 0) textcolor(14); else textcolor(8);
  txt_write("1");
  gotoxy(50,10); if (player.party[2].hp != 0) textcolor(14); else textcolor(8);
  txt_write("2");
  gotoxy(20,11); if (player.party[3].hp != 0) textcolor(14); else textcolor(8);
  txt_write("3");
  gotoxy(60,11); if (player.party[4].hp != 0) textcolor(14); else textcolor(8);
  txt_write("4");
  gotoxy(40,11); if (player.party[5].hp != 0) textcolor(14); else textcolor(8);
  txt_write("5");
  }

static void show_member_stats(Int32 member) {
  char buffer[80];
  textcolor(2);
  gotoxy(20,16); txt_write("Name   :");
  gotoxy(20,17); txt_write("Level  :");
  gotoxy(20,18); txt_write("Hp     :");
  gotoxy(20,19); txt_write("Mana   :");
  gotoxy(20,20); txt_write("Attack :");
  gotoxy(20,21); txt_write("Defense:");
  gotoxy(20,22); txt_write("Orders :");
  gotoxy(40,17); txt_write("Time      :");
  gotoxy(40,18); txt_write("Sleeping  :");
  gotoxy(40,19); txt_write("Scared    :");
  gotoxy(40,20); txt_write("Experience:");
  gotoxy(40,21); txt_write("Next Level:");
  textcolor(14);
  gotoxy(28,16); txt_write(player.party[member].name);
  sprintf(buffer,"%d",player.party[member].level);
  gotoxy(28,17); txt_write(buffer);
  sprintf(buffer,"%d",player.party[member].hp);
  gotoxy(28,18); txt_write(buffer);
  sprintf(buffer,"%d",player.party[member].mana);
  gotoxy(28,19); txt_write(buffer);
  sprintf(buffer,"%d",player.party[member].attack);
  gotoxy(28,20); txt_write(buffer);
  sprintf(buffer,"%d",player.party[member].defense);
  gotoxy(28,21); txt_write(buffer);
  gotoxy(28,22);
  if ((player.party[member].special & MS_EVADE) == 0)
    txt_write("Attack"); else txt_write("Evade ");
  gotoxy(51,17);
  if (player.party[member].time < 0)
    txt_write("Perm");
  else {
    sprintf(buffer,"%d      ",player.party[member].time);
    txt_write(buffer);
    }
   gotoxy(51,18);
  if (player.party[member].sleep == 0)
    txt_write("No "); else txt_write("Yes ");
   gotoxy(51,19);
  if (player.party[member].fear == 0)
    txt_write("No "); else txt_write("Yes ");
  gotoxy(51,20);
  sprintf(buffer,"%9.0f",player.party[member].exp);
  txt_write(buffer);
  gotoxy(51,21);
  sprintf(buffer,"%9.0f",needed(player.party[member].level+1));
  txt_write(buffer);
  }

static void show_member(Int32 member,MENU* menu) {
  Int32  i;
  char key;
  char flag;
  char buffer[80];
  char training[80];
  double cost;

  flag = ' ';
  for (i=16; i<=22; i++) {
    gotoxy(28,i);
    txt_write("                                                ");
    }
  while (flag == ' ') {
    flag = '*';
      show_member_stats(member);
    if (player.in_city == 'Y') {
      cost = (needed(player.party[member].level + 1) - player.party[member].exp);
      if (cost <= get_gold()) {
        sprintf(training,"Training to next level will cost %.0f Gold",cost);
        sprintf(buffer,"Gold=%d",get_gold());
        key = ' ';
        if (improved_interface == 'Y') {
          write_centred(25,14,buffer);
          show_menu_question(menu,14,training);
          key = ask_action(0,menu->keys_line + 1,14,"Train","Cancel",0);
          draw_menu_keys(menu->keys,menu->keys_line);
          }
        else {
          gotoxy(20,23); txt_write(training);
          gotoxy(20,24); txt_write("Do you wish to train member ?");
          gotoxy(34,25); txt_write(buffer);
          while (key != 'Y' && key != 'N') {
            key = inkey();
            if (key == 'n') key = 'N';
            if (key == 'y') key = 'Y';
            }
          }
        if (key == 'Y') {
          player.party[member].exp += cost + 1;
          player.party[member].level++;
          player.party[member].attack += 2;
          player.party[member].defense += 2;
          if (player.party[member].mana != 0)
            player.party[member].mana += 25;
          sub_gold(cost);
          player.party[member].hp = player.party[member].level * 9;
          party_show_top(menu);
          show_member_stats(member);
          }
        }
      else
        flag = '*';
        gotoxy(20,23); txt_write("                                                  ");
        gotoxy(20,24); txt_write("                                                  ");
        gotoxy(20,25); txt_write("                                                  ");
      }
    }
  }

/* The key of the orders picked with the cursor, which starts on the member's own; ESC keeps them */
static char pick_orders(Int32 member,MENU* menu) {
  Int32 row;

  hide_menu_keys(menu);
  row = ask_choice(0,menu->keys_line,10,order_names,NUM_ORDERS,(player.party[member].special & MS_EVADE) ? 1 : 0);
  draw_menu_keys(menu->keys,menu->keys_line);

  return (row >= 0) ? "ae"[row] : 0;
  }

static void set_orders(Int32 member,MENU* menu) {
  char key;

  key = ' ';
  if (improved_interface == 'Y') key = pick_orders(member,menu);
  else {
    gotoxy(20,14); textcolor(10);
    txt_write("<A>ttack, or <E>vade ?");
    while (key != 'a' && key != 'e') key = inkey();
    }

  switch (key) {
    case 'a':player.party[member].special &= (~MS_EVADE);
             break;
    case 'e':player.party[member].special |= MS_EVADE;
             break;
    }

  gotoxy(20,14); textcolor(10);
  txt_write("                          ");
  }

static void swap_members(Int32 i,Int32 j,MENU* menu) {
  NPC_REC ptemp;

  ptemp = player.party[i];
  player.party[i] = player.party[j];
  player.party[j] = ptemp;

  party_show_top(menu);
  }

/* The member picked with the cursor after the question, or 0 when ESC backs out */
static Int32 pick_member(MENU* menu,char* question) {
  Int32 row;

  show_menu_question(menu,10,question);
  row = menu_choice(menu);
  draw_menu_keys(menu->keys,menu->keys_line);

  return row + 1;
  }

/* A key on the party screen with the Improved Interface preference: the cursor picks the member in place of 1 to 5 */
static void party_command(MENU* menu,char key) {
  Int32 member;
  Int32 other;

  if (key == 13 && player.party[menu->row + 1].hp != 0) show_member(menu->row + 1,menu);
  if (key == 'd' && player.in_city == 'Y') {
    member = pick_member(menu,"Dismiss who?");
    if (member > 0) {
      player.party[member].hp = 0;
      party_show_top(menu);
      }
    }
  if (key == 'o') {
    member = pick_member(menu,"Change who's orders?");
    if (member > 0) set_orders(member,menu);
    }
  if (key == 'c') {
    member = pick_member(menu,"Change who's position?");
    other = (member > 0) ? pick_member(menu,"Change to what position?") : 0;
    if (other > 0) swap_members(member,other,menu);
    }
  }

void party() {
  Int32 i=0,j=0;
  char temp;
  char key;
  MENU menu = { .rows = { .count = 5, .shown = 5, .line = 2 }, .column = 21, .value_column = 49, .color = 14,
                .keys = PARTY_KEYS, .keys_line = 13, .draw_row = draw_member };

  saveScreen();
  textmode(3); party_show_top(&menu);
  if (improved_interface != 'Y') {
    gotoxy(10,13); textcolor(3);
    txt_write("<D>ismiss, <1-5>Statistics, <C>hange order, <O>orders, <Q>uit");
    }

  key = ' ';
  while (key != 'q') {
    key = (improved_interface == 'Y') ? menu_key(&menu) : inkey();
    if (key == 27) key = 'q';
    if (improved_interface == 'Y') {
      party_command(&menu,key);
      continue;
      }
    switch (key) {
      case '1':if (player.party[1].hp != 0) show_member(1,&menu); break;
      case '2':if (player.party[2].hp != 0) show_member(2,&menu); break;
      case '3':if (player.party[3].hp != 0) show_member(3,&menu); break;
      case '4':if (player.party[4].hp != 0) show_member(4,&menu); break;
      case '5':if (player.party[5].hp != 0) show_member(5,&menu); break;
      case 'd':if (player.in_city == 'Y') {
                 gotoxy(20,14); textcolor(10);
                 txt_write("Dismiss who ?");
                 key = ' ';
                 while (key < '0' || key > '5') key = inkey();
                 if (key > '0') {
                   switch (key) {
                     case '1':player.party[1].hp = 0; break;
                     case '2':player.party[2].hp = 0; break;
                     case '3':player.party[3].hp = 0; break;
                     case '4':player.party[4].hp = 0; break;
                     case '5':player.party[5].hp = 0; break;
                     }
                   party_show_top(&menu);
                   }
                 gotoxy(20,14); textcolor(10);
                 txt_write("             ");
                 }
               break;
      case 'o':gotoxy(20,14); textcolor(10);
               txt_write("Change whos orders ?");
               key = ' ';
               while (key < '0' || key > '5') key = inkey();
               temp = key;
               gotoxy(20,14); textcolor(10);
               txt_write("                    ");
               switch (temp) {
                 case '1':i = 1; break;
                 case '2':i = 2; break;
                 case '3':i = 3; break;
                 case '4':i = 4; break;
                 case '5':i = 5; break;
                 }
               set_orders(i,&menu);
               break;
      case 'c':gotoxy(20,14); textcolor(10);
               txt_write("Change whos position ?");
               key = ' ';
               while (key < '0' || key > '5') key = inkey();
               temp = key;
               gotoxy(20,14); textcolor(10);
               txt_write("                      ");
               gotoxy(20,14); textcolor(10);
               txt_write("Change to what position ?");
               key = ' ';
               while (key < '0' || key > '5') key = inkey();
               gotoxy(20,14); textcolor(10);
               txt_write("                         ");
               switch (temp) {
                 case '1':j = 1; break;
                 case '2':j = 2; break;
                 case '3':j = 3; break;
                 case '4':j = 4; break;
                 case '5':j = 5; break;
                 }
               switch (key) {
                 case '1':i = 1; break;
                 case '2':i = 2; break;
                 case '3':i = 3; break;
                 case '4':i = 4; break;
                 case '5':i = 5; break;
                 }
               swap_members(i,j,&menu);
               break;
      }
    }
  check_mount();
  restoreScreen();
/*  show_screen(); */
  }

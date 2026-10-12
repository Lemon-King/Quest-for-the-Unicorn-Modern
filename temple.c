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

void rebirth() {
  char key;
  clrscr();
  textcolor(15);
  gotoxy(10,10); txt_write("The Ritual of Rebirth has begun.  The druids begin to chant");
  gotoxy(10,11); txt_write("and your world grows dark.  You hear a voice in the void, ");
  gotoxy(10,12); txt_write("\"You are reborn, your journey begins anew.\"");
  textcolor(7);
  gotoxy(10,16); txt_write("Select game mode: <N>ormal, <H>eroic, <E>pic: ");
  key = ' ';
  while (key != 'N' && key != 'H' && key != 'E') {
    key = inkey();
    if (key == 'n') key = 'N';
    if (key == 'h') key = 'H';
    if (key == 'e') key = 'E';
    }
  if (key == 'N') heroicMode = 0;
  if (key == 'H') heroicMode = 2;
  if (key == 'E') heroicMode = 4;
  }

#define NUM_SERVICES    7

/* The services, picked by '1' to '7' in the numbered menu */
static char* service_names[NUM_SERVICES] = {
  "Healing", "Prayer", "Restore allignment", "Remove Curse", "Become a Hierophant", "Rebirth", "One with Nature"
  };

static double alignment_cost(void) {
  double cost;
  cost = player.align;
  return 500*(100-cost);
  }

/* The price shown beside a service, blank for those priced later */
static void service_price(char service,char* text) {
  strcpy(text,"");
  if (service == '1') sprintf(text,"%5.0f",100.0);
  if (service == '2') sprintf(text,"%5.0f",500.0);
  if (service == '3') sprintf(text,"%5.0f",alignment_cost());
  }

/* The keys of the services offered, in order. Returns how many. */
static Int32 offered_services(char allowed,char* services) {
  Int32 count;
  count = 0;
  services[count++] = '1';
  services[count++] = '2';
  services[count++] = '3';
  services[count++] = '4';
  if (allowed == 'Y') services[count++] = '5';
  if (player.gateway == 'F' && player.effect[PE_HEROIC] >= 0) services[count++] = '6';
  if (player.effect[PE_HEROIC] >= 0) services[count++] = '7';
  return count;
  }

static void draw_service(MENU* menu,Int32 row,Int32 line) {
  char service;
  char price[16];
  service = ((char*)menu->screen)[row];
  service_price(service,price);
  draw_menu_text(menu,line,service_names[service - '1'],price);
  }

static void show_numbered(char* services,Int32 count) {
  Int32 i;
  Int32 line;
  char buffer[80];
  line = 3;
  for (i=0; i<count; i++) {
    sprintf(buffer,"%c> %s",services[i],service_names[services[i] - '1']);
    gotoxy(27,line); txt_write(buffer);
    service_price(services[i],buffer);
    gotoxy(51,line); txt_write(buffer);
    line++;
    }
  gotoxy(27,line++); txt_write("<ESC> Leave");
  line++;
  gotoxy(30,line); txt_write("Option ?");
  }

static void show_top(char* name,char allowed,double* cost,MENU* menu) {
  char buffer[80];
  char services[NUM_SERVICES];
  Int32 count;
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);
  textcolor(14);
  *cost = alignment_cost();
  count = offered_services(allowed,services);
  if (cursor_menus == 'Y') {
    memcpy(menu->screen,services,count);
    fit_menu(menu,count);
    draw_menu(menu);
    }
  else show_numbered(services,count);
  textcolor(15); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

/* The key of the service picked, or ESC */
static char pick_service(MENU* menu) {
  Int32 row;
  row = menu_choice(menu);
  return (row >= 0) ? ((char*)menu->screen)[row] : 27;
  }

void temple(char* name) {
  double cost;
  char   buffer[80];
  char   key;
  OBJECT_REC* item;
  Int32    hier;
  Int32    i;
  char   allowed;
  char   services[NUM_SERVICES];
  MENU   menu = { .rows = { .shown = NUM_SERVICES, .line = 2 }, .column = 26, .value_column = 51,
                  .color = 14, .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_service, .screen = services };

  hier = -1;
  allowed = 'N';
  for (i=0; i<NUM_CLASSES; i++)
    if (classes[i].flags & CF_HIER) hier = i + 1;
  if (hier >= 0) allowed = (check_class_change(hier)) ? 'Y' : 'N';
  show_top(name,allowed,&cost,&menu);
  key = ' ';
  while (key != '0') {
    key = (cursor_menus == 'Y') ? pick_service(&menu) : inkey();
    if (key == 27) key = '0';
    if (key == '1' && get_gold()>=100) {
      player.chp = (classes[player.class-1].flags & CF_DRUID) ? player.mhp * 2 : player.mhp;
      sub_gold(100);
      textcolor(11);
      gotoxy(27,12); txt_write("Healed");
      delay(1000);
      }
    if (key == '2' && get_gold()>=500) {
      player.cmana = (classes[player.class-1].flags & CF_DRUID) ? player.mmana * 2 : player.mmana;
      if (classes[player.class-1].flags & CF_HIER) player.cmana = player.mmana * 2;
      sub_gold(500);
      if (player.mmana != 0) {
        textcolor(11);
        gotoxy(27,12); txt_write("Mana Increased");
        delay(1000);
        }
      }
    if (key == '3' && get_gold()>cost) {
      sub_gold(cost);
      player.align = 100;
      textcolor(11);
      gotoxy(27,12); txt_write("Alignment Restored");
      delay(1000);
      }
    if (key == '4') {
      item = inventory(NULL);
      show_top(name,allowed,&cost,&menu);

      /* Without the Temple Uncurse change choosing no item crashes on item->plus */
      if (item == NULL && change_temple_uncurse == 'Y') {
        gotoxy(27,13);
        textcolor(15);
        txt_write("Maybe next time.");
        delay(1000);
        }
      else if (item->plus >= 0) {
        gotoxy(27,13); textcolor(9); txt_write("That Item is not cursed");
        delay(1000);
        } else {
        cost = abs(item->plus) * 1000;
        sprintf(buffer,"Item will cost %.0f gold to uncurse",cost);
        gotoxy(27,13); textcolor(15); txt_write(buffer);
        if (cost > get_gold()) {
          gotoxy(27,14); textcolor(12); txt_write("You do not have enough gold!!!");
          } else {
          gotoxy(27,15); textcolor(11); txt_write("Do you wish to uncurse it? (Y/N)");
          while (key != 'Y' && key != 'N') {
            key = inkey();
            if (key == 'n') key = 'N';
            if (key == 'y') key = 'Y';
            }
          if (key == 'Y') {
            item->plus = 0;
            sub_gold(cost);
            gotoxy(27,16); textcolor(9); txt_write("Curse has been removed.");
            } else {
            gotoxy(27,16); textcolor(15); txt_write("Maybe next time.");
            }
          }
        delay(2500);
        }
      }
    if (allowed == 'Y' && key == '5') {
      make_h();
      textcolor(11);
      gotoxy(27,12); txt_write("Now a Hierophant");
      delay(1000);
      }
    if (player.gateway == 'F' && player.effect[PE_HEROIC] >=0 && key == '6') {
      textcolor(12); gotoxy(22,14); txt_write("WARNING! Rebirth will clear your game!");
      textcolor(15); gotoxy(18,16); txt_write("Do you wish to perform the Ritual of Rebirth?");
      key = ' ';
      while (key != 'Y' && key != 'N') {
        key = inkey();
        if (key == 'n') key = 'N';
        if (key == 'y') key = 'Y';
        }
      if (key == 'Y') { rebirth(); exitf = 7; key = '0'; }
      }
    if (player.effect[PE_HEROIC] >=0 && key == '7') {
      if (player.effect[PE_HEROIC] == 0) { textcolor(12); gotoxy(22,14); txt_write("WARNING! This will downgrade your game to Casual!"); }
      if (player.effect[PE_HEROIC] == 2) { textcolor(12); gotoxy(22,14); txt_write("WARNING! This will downgrade your game to Normal!"); }
      if (player.effect[PE_HEROIC] == 4) { textcolor(12); gotoxy(22,14); txt_write("WARNING! This will downgrade your game to Heroic!"); }
      textcolor(15); gotoxy(18,16); txt_write("Do you wish to become one with nature?");
      while (key != 'Y' && key != 'N') {
        key = inkey();
        if (key == 'n') key = 'N';
        if (key == 'y') key = 'Y';
        }
      if (key == 'Y') {
        if (player.effect[PE_HEROIC] == 0) player.effect[PE_HEROIC] = -1;
        if (player.effect[PE_HEROIC] == 2) player.effect[PE_HEROIC] = 0;
        if (player.effect[PE_HEROIC] == 4) player.effect[PE_HEROIC] = 2;
        if (player.effect[PE_HEROIC] < 0) end_permadeath();
        }
      }
    show_top(name,allowed,&cost,&menu);
    }
  } /* void temple(char* name) */

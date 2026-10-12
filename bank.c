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

#define NUM_ACCOUNT_OPTIONS  2

/* Picked by '1' and '2' in the numbered menu */
static char* account_options[NUM_ACCOUNT_OPTIONS] = { "Deposit", "Withdrawal" };

static void draw_account_option(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,account_options[row],"");
  }

static void show_top(char* name,MENU* menu) {
  Int32 i;
  Int32 l;
  char buffer[80];
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);
  sprintf(buffer,"Your account contains %d gold",player.bank);
  gotoxy(25,4); txt_write(buffer);
  sprintf(buffer,"Interest paid last period: %d gold",player.interest);
  gotoxy(25,5); txt_write(buffer);
  textcolor(11); gotoxy( 15,7);
  txt_write("House               Principle    Interest    Payment");
  l = 8; textcolor(2);
  for (i=0; i<5; i++) {
    if (player.houses[i].region != 0) {
      gotoxy(15,l); txt_write(link_name(player.houses[i].region,
                                        player.houses[i].rx,
                                        player.houses[i].ry));
      sprintf(buffer,"%.0f",player.houses[i].mortgage);
      gotoxy(35,l); txt_write(buffer);
      sprintf(buffer,"%.0f",player.houses[i].interest);
      gotoxy(48,l); txt_write(buffer);
      sprintf(buffer,"%.0f",player.houses[i].payment);
      gotoxy(60,l); txt_write(buffer);
      }
    }

  textcolor(14);
  if (improved_interface == 'Y') draw_menu(menu);
  else {
    for (i=0; i<NUM_ACCOUNT_OPTIONS; i++) {
      sprintf(buffer,"%d> %s",i+1,account_options[i]);
      gotoxy(34,14+i); txt_write(buffer);
      }
    gotoxy(34,16); txt_write("<ESC> Leave");
    gotoxy(37,17); txt_write("Option ?");
    }

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

void bank(char* name) {
  double amt;
  char   buffer[80];
  char   key;
  MENU   menu = { .rows = { .count = NUM_ACCOUNT_OPTIONS, .shown = NUM_ACCOUNT_OPTIONS, .line = 13 },
                  .column = 33, .value_column = 48, .color = 14, .keys = MENU_KEYS, .keys_line = 25,
                  .draw_row = draw_account_option };

  show_top(name,&menu);
  key = ' ';
  while (key != '0') {
    key = (improved_interface == 'Y') ? menu_number(&menu) : inkey();
    if (key == 27) key = '0';
    if (key == '1' && get_gold() > 0) {
      gotoxy(30,19); textcolor(14);
      txt_write("How much to deposit ?");
      strcpy(buffer,"");
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%lf",&amt);
        } else amt = 0;
      amt = typed_gold(amt);
      if (amt <= get_gold()) {
        player.bank += amt;
        sub_gold(amt);
        show_top(name,&menu);
        } else {
        gotoxy(30,20); textcolor(12);
        txt_write("You do not have that much!!!");
        delay(5000);
        show_top(name,&menu);
        }
      }
    if (key == '2' && player.bank > 0) {
      gotoxy(30,19); textcolor(14);
      txt_write("How much to withdraw ?");
      strcpy(buffer,"");
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%lf",&amt);
        } else amt = 0;
      amt = typed_gold(amt);
      if (amt <= player.bank) {
        player.bank -= amt;
        add_gold(amt);
        show_top(name,&menu);
        } else {
        gotoxy(25,20); textcolor(12);
        txt_write("Your account does not have that much!!!");
        delay(5000);
        show_top(name,&menu);
        }
      }
    show_top(name,&menu);
    }
  } /* void temple(char* name) */

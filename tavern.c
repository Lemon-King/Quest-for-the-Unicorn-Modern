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

#define NUM_FARE       10

/* The tavern's fare, picked by 1 to 10 in the numbered menu, with its price; the gamble has none */
typedef struct {
  char*  name;
  Int32  price;
  } FARE_REC;

static FARE_REC fare[NUM_FARE] = {
  { "Merchants Meal",  4 },
  { "Rich Meal",       6 },
  { "Beer",            2 },
  { "Ale",             2 },
  { "Mead",            2 },
  { "Wine",            2 },
  { "Room, 1 night",  20 },
  { "Room, 1 Week",  120 },
  { "Room, 1 Month", 500 },
  { "Gamble",          0 }
  };

static void move_player(void) {
  switch (player.city) {
    case 14:
         if (player.x>51 && player.x<55 && player.y>15 && player.y<19)
           switch (u_random(5)) {
             case 0:
                 player.x = 40;
                 player.y = 17+2*u_random(4);
                 break;
             case 1:
                 player.x = 61;
                 player.y = 17+2*u_random(4);
                 break;
             case 2:
                 player.y = 33;
                 player.x = 41+2*u_random(11);
                 break;
             case 3:
                 player.y = 25;
                 player.x = 40+2*u_random(5);
                 break;
             case 4:
                 player.y = 25;
                 player.x = 53+2*u_random(5);
                 break;
             }
         break;
    case 13:
         if (player.x>57 && player.x<61 && player.y>11 && player.y<15)
           switch (u_random(3)) {
             case 0:
                 player.y = 18;
                 player.x = 52+3*u_random(5);
               break;
             case 1:
                 player.y = 24;
                 player.x = 52+3*u_random(5);
               break;
             case 2:
                 player.y = 26;
                 player.x = 52+3*u_random(5);
               break;
             }
         break;
    case 17:
         if (player.x>41 && player.x<45 && player.y>20 && player.y<24) {
           player.y = 22;
           player.x = 12+2*u_random(12);
           }
         break;
    case 16:
         if (player.x>26 && player.x<30 && player.y>52 && player.y<56)
           switch (u_random(2)) {
             case 0:
                 player.y = 62;
                 player.x = 9+2*u_random(8);
               break;
             case 1:
                 player.y = 70;
                 player.x = 9+2*u_random(10);
               break;
             }
         if (player.x>18 && player.x<22 && player.y>82 && player.y<86)
           switch (u_random(4)) {
             case 0:
                 player.y = 74;
                 player.x = 18+2*u_random(7);
               break;
             case 1:
                 player.y = 82;
                 player.x = 22+2*u_random(5);
               break;
             case 2:
                 player.x = 9;
                 player.y = 74+2*u_random(10);
               break;
             case 3:
                 player.x = 17;
                 player.y = 86+2*u_random(4);
               break;
             }
         break;
       }
  player.map_x = player.x-8;
  player.map_y = player.y-8;
  if (player.map_x<1) player.map_x = 1;
  if (player.map_y<1) player.map_y = 1;
  if (player.map_x>84) player.map_x = 84;
  if (player.map_y>84) player.map_y = 84;
  restoreScreen();
  show_screen();
  saveScreen();
  }

/* Fare i's price as width digits, blank for the gamble */
static void fare_price(Int32 i,Int32 width,char* text) {
  if (fare[i].price > 0) sprintf(text,"%*d",width,fare[i].price);
    else sprintf(text,"%*s",width,"");
  }

static void draw_fare(MENU* menu,Int32 row,Int32 line) {
  char price[16];

  fare_price(row,5,price);
  draw_menu_text(menu,line,fare[row].name,price);
  }

/* The fare in two columns, 1 to 6 and 7 to 10 */
static void show_numbered(void) {
  Int32 i;
  char  price[16];
  char  left[48];
  char  right[48];
  char  buffer[100];

  gotoxy(5,3); textcolor(14);
  for (i=0; i<6; i++) {
    fare_price(i,1,price);
    sprintf(left,"%d> %-26s%s",i+1,fare[i].name,price);

    if (i + 6 < NUM_FARE) {
      fare_price(i+6,5,price);
      sprintf(right,"  %2d> %-27s%s",i+7,fare[i+6].name,price);
      }
    else sprintf(right,"%38s","");

    sprintf(buffer,"%s%s%s",(i == 0) ? "" : "    ",left,right);
    txt_writeln(buffer);
    }
  }

static void show_top(char* name,MENU* menu) {
  char buffer[80];

  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  if (improved_interface == 'Y') draw_menu(menu);
    else show_numbered();

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

void tavern(char* name) {
  Int32 option;
  double price;
  Int32  i;
  char buffer[80];
  MENU menu = { .rows = { .count = NUM_FARE, .shown = NUM_FARE, .line = 2 }, .column = 26, .value_column = 51,
                .color = 14, .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_fare };

  show_top(name,&menu);
  option = 1;
  price = 4;
  while (option != 0) {
    gotoxy(15,17); textcolor(10);
    if (improved_interface == 'Y') {
      txt_write("Which item would you like (<ESC> to leave) ?");
      option = menu_choice(&menu) + 1;
      }
    else {
      txt_write("Which item would you like (<ESC> to leave) ?        ");
      gotoxy(60,17); textcolor(15);
      strcpy(buffer,"");
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%d",&option);
        } else option = 0;
      }
    for (i=17; i<=22; i++) {
      gotoxy(10,i); txt_write("                                                                    ");
      }
    if (option == 10) {
      gamble();
      show_top(name,&menu);
      }
    if (option>0 && option<10) {
      price = fare[option - 1].price;
      if (price>get_gold()) {
        gotoxy(15,19); textcolor(12);
        txt_write("But you do not have enough gold!");
        }
      else {
        sub_gold(price);
        switch (option) {
          case 1:
                if (player.health<100) player.health = player.health+10;
                if (player.chp<player.mhp) player.chp = player.chp+10;
                 break;
          case 2:
                 if (player.health<100) player.health = player.health+20;
                 if (player.chp<player.mhp) player.chp = player.chp+20;
                 break;
          case 3:
                 if (player.health<100) player.health = player.health+3;
                 if (player.chp<player.mhp) player.chp = player.chp+1;
                 break;
          case 4:
                 if (player.health<100) player.health = player.health+3;
                 if (player.chp<player.mhp) player.chp = player.chp+1;
                 break;
          case 5:
                 if (player.health<100) player.health = player.health+3;
                 if (player.chp<player.mhp) player.chp = player.chp+1;
                 break;
          case 6:
                 if (player.health<100) player.health = player.health+3;
                 if (player.chp<player.mhp) player.chp = player.chp+1;
                 break;
          case 7:
                 if (player.health<100) player.health = 100;
                 if (player.chp<player.mhp) player.chp = player.mhp;
                 if (player.cmana<player.mmana) player.cmana = player.mmana;
                 move_player();
                 player.day = player.day+1;
                 if (player.day>31) {
                   new_month();
/*
                   player.day = player.day-31;
                   player.month = player.month+1;
                   player.interest = player.bank * .005;
                   player.bank += player.interest;
                   if (player.month == 13) {
                     player.month = 1;
                     player.age = player.age+1;
                     player.cav_paid = 'N';
                     }
*/
                   }
                 option = 0;
                 break;
          case 8:
                 if (player.health<100) player.health = 100;
                 player.chp = player.mhp*2;
                 if (player.cmana<player.mmana) player.cmana = player.mmana;
                 move_player();
                 player.day = player.day+7;
                 if (player.day>31) {
                   new_month();
/*
                   player.day = player.day-31;
                   player.month = player.month+1;
                   player.interest = player.bank * .005;
                   player.bank += player.interest;
                   if (player.month == 13) {
                     player.month = 1;
                     player.age = player.age+1;
                     player.cav_paid = 'N';
                     }
*/
                   }
                   option = 0;
                 break;
          case 9:
                 if (player.health<100) player.health = 100;
                 player.chp = player.mhp*3;
                 if (player.cmana<player.mmana) player.cmana = player.mmana;
                 move_player();
                 player.day = player.day+31;
                 if (player.day>31) {
                   new_month();
/*
                   player.day = player.day-31;
                   player.month = player.month+1;
                   player.interest = player.bank * .005;
                   player.bank += player.interest;
                   if (player.month == 13) {
                     player.month = 1;
                     player.age = player.age+1;
                     player.cav_paid = 'N';
                     }
*/
                   }
                 option = 0;
                 break;
          }
        show_top(name,&menu);
        }
      }
    }
  } /* void tavern(char* name) */

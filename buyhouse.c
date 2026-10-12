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
#include "math.h"

#define NUM_PURCHASE_OPTIONS  2

/* Picked by '1' and '2' in the numbered menu */
static char* purchase_options[NUM_PURCHASE_OPTIONS] = { "Change Down Payment", "Purchase House" };

static void draw_purchase_option(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,purchase_options[row],"");
  }

static void show_top(char* buffer,Int32 hnum,double down,double a,double ccost,MENU* menu) {
  Int32 i;
  textmode(3); gotoxy(33,1); textcolor(11);
  txt_write("Purchase House");
  textcolor(14);
  sprintf(buffer,"%.0f",forsale[hnum].price);
  gotoxy(24,3); txt_write("Price"); gotoxy(51,3); txt_write(buffer);
  sprintf(buffer,"%.0f",down);
  gotoxy(24,4); txt_write("Down Payment");  gotoxy(51,4); txt_write(buffer);
  sprintf(buffer,"%.0f",forsale[hnum].price-down);
  gotoxy(24,5); txt_write("Financed Amount");  gotoxy(51,5); txt_write(buffer);
  sprintf(buffer,"%.0f",a);
  gotoxy(24,6); txt_write("Monthly Payment");  gotoxy(51,6); txt_write(buffer);
  sprintf(buffer,"%.0f",ccost);
  gotoxy(24,7); txt_write("Closing Costs");  gotoxy(51,7); txt_write(buffer);
  sprintf(buffer,"%.0f",ccost+down);
  gotoxy(24,8); txt_write("Due at Closing");  gotoxy(51,8); txt_write(buffer);

  if (cursor_menus == 'Y') draw_menu(menu);
  else {
    for (i=0; i<NUM_PURCHASE_OPTIONS; i++) {
      sprintf(buffer,"%d> %s",i+1,purchase_options[i]);
      gotoxy(27,10+i); txt_write(buffer);
      }
    gotoxy(27,12); txt_write("<ESC> Leave");
    gotoxy(30,13); txt_write("Option ?");
    }

  textcolor(15); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

void buy_house(Int32 hnum) {
  double down;
  double ccost;
  double p;
  double a;
  double i;
  char   buffer[80];
  char   key;
  Int32    flag;
  Int32    j;
  MENU   menu = { .rows = { .count = NUM_PURCHASE_OPTIONS, .shown = NUM_PURCHASE_OPTIONS, .line = 9 },
                  .column = 26, .value_column = 50, .color = 14, .keys = MENU_KEYS, .keys_line = 25,
                  .draw_row = draw_purchase_option };

  p = forsale[hnum].price;
  i = .005;
  a = p*((i*pow(1+i,360))/(pow(1+i,360)-1));
  ccost = forsale[hnum].price * 0.023;
  down = 0;
  show_top(buffer,hnum,down,a,ccost,&menu);
  key = ' ';
  while (key != '0') {
    key = (cursor_menus == 'Y') ? menu_number(&menu) : inkey();
    if (key == 27) key = '0';
    if (key == '1') {
      gotoxy(27,15); textcolor(2); txt_write("New Down Payment: ");
      textcolor(15);
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%lf",&down);
        } else down = 0;
      down = typed_gold(down);
      a = (p-down)*((i*pow(1+i,360))/(pow(1+i,360)-1));
      }
    if (key == '2') {
      if (ccost+down > get_gold()) {
        gotoxy(27,15); textcolor(4); txt_write("You do not have enough gold ");
        delay(5000);
        }
      else {
        flag = -1;
        for (j=0; j<5; j++)
          if (flag == -1 && player.houses[j].region == 0) flag = j;
        if (flag < 0) {
          gotoxy(27,15); textcolor(4); txt_write("You already own the maximum number of houses ");
          delay(5000);
          }
        else {
          player.houses[flag].region = forsale[hnum].region;
          player.houses[flag].rx = forsale[hnum].rx;
          player.houses[flag].ry = forsale[hnum].ry;
          player.houses[flag].cx = forsale[hnum].cx;
          player.houses[flag].cy = forsale[hnum].cy;
          player.houses[flag].mortgage = forsale[hnum].price - down;
          player.houses[flag].interest = 0;
          player.houses[flag].payment = a;
          for (j=0; j<500; j++) player.houses[flag].inventory[j].otype = O_NONE;
          sub_gold(down+ccost);
          player.map[forsale[hnum].cx][forsale[hnum].cy] = S_SOLD;
          key = '0';
          restoreScreen();
          show_spot(forsale[hnum].cx,forsale[hnum].cy);
          saveScreen();
          }
        }
      }
    show_top(buffer,hnum,down,a,ccost,&menu);
    }
  } /* void temple(char* name) */

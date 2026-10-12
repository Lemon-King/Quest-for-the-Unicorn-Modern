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

#define NUM_COURSES    24

typedef struct {
  char name[32];
  Int32  cost;
  Int32  skill;
  } SCHOOL_CLASSES;

SCHOOL_CLASSES school_classes[] = {
  { "Dagger Weaponry     ",   50, 17 },
  { "1H Sword Weaponry   ",   50, 2 },
  { "2H Sword Weaponry   ",   50, 18 },
  { "Axe Weaponry        ",   50, 19 },
  { "Flail Weaponry      ",   50, 20 },
  { "Spear Weaponry      ",   50, 3 },
  { "Trident Weaponry    ",   50, 22 },
  { "Poleaxe Weaponry    ",   50, 23 },
  { "Mace Weaponry       ",   50, 4 },
  { "Bow Weaponry        ",   50, 1 },
  { "Crossbow Weaponry   ",   50, 21 },
  { "Thrown Weaponry     ",   50, 24 },
  { "Scouting            ",   35, 5 },
  { "Communication       ",   25, 6 },
  { "Magical Writing     ",  200, 7 },
  { "Magical Incantation ",  200, 8 },
  { "Medical Practices   ",   75, 9 },
  { "Aclchemy            ",   97, 10 },
  { "Horsemanship        ",   25, 11 },
  { "Nautical Navigation ",   45, 12 },
  { "Psychic Awareness   ",  250, 13 },
  { "Public Relations    ",   32, 14 },
  { "Wilderness Survival ",   35, 15 },
  { "Combat Techniques   ",   50, 16 },
  };

static Int32 class_price(Int32 cls) {
  Int32 ret;
  Int32 skill;
  ret = school_classes[cls].cost;
  skill = school_classes[cls].skill;
  if (get_skill(skill) >= 5 && get_skill(skill) < 10) ret *= 3;
  if (get_skill(skill) >= 10 && get_skill(skill) < 25) ret *= 5;
  if (get_skill(skill) >= 25 && get_skill(skill) < 50) ret *= 7;
  if (get_skill(skill) >= 50 && get_skill(skill) < 75) ret *= 9;
  if (get_skill(skill) >= 75)  ret *= 11;
  ret *= (1 + get_skill(school_classes[cls].skill));
  return ret;
  }

static void draw_course(MENU* menu,Int32 row,Int32 line) {
  char price[16];

  sprintf(price,"%7d",class_price(row));
  draw_menu_text(menu,line,school_classes[row].name,price);
  }

static void show_top(char* name,MENU* menu) {
  Int32 i;
  char buffer[80];

  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  gotoxy(1,3); textcolor(14);
  if (cursor_menus == 'Y') draw_menu(menu);
  else {
    for (i=0; i<12; i++) {
      sprintf(buffer,"   %2d> %s  %7d      %2d> %s  %7d",i+1,school_classes[i].name,class_price(i),
                                                         i+13,school_classes[i+12].name,class_price(i+12));
      txt_writeln(buffer);
      }
    }

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  }

void school(char* name) {
  Int32 option;
  double price;
  Int32 i;
  char buffer[80];
  MENU menu = { .rows = { .count = NUM_COURSES, .shown = 12, .line = 2, .more_column = 52 },
                .column = 26, .value_column = 52, .color = 14, .keys = MENU_KEYS, .keys_line = 25,
                .draw_row = draw_course };

  show_top(name,&menu);
  option = 1;
  price = 5000;
  while (option != 0) {
    gotoxy(15,17); textcolor(10);
    if (cursor_menus == 'Y') {
      txt_write("Which course would you like to take (<ESC> to leave) ?");
      option = menu_choice(&menu) + 1;
      }
    else {
      txt_write("Which course would you like to take (<ESC> to leave) ?        ");
      gotoxy(70,17); textcolor(15);
      strcpy(buffer,"");
      if (p_read(buffer) == 0) {
        sscanf(buffer,"%d",&option);
        } else option = 0;
      }
    for (i=17; i<=22; i++) {
      gotoxy(10,i); txt_write("                                                              ");
      }
    if (option>0 && option<25) {
      price = class_price(option-1);
      if (price>get_gold()) {
        gotoxy(15,19); textcolor(12);
        txt_write("But you do not have enough gold for that course!");
        }
      else {
        sub_gold(price);
        gotoxy(15,19); textcolor(15);
        txt_write("Course Taken!!");
        stats.counts[STAT_CLASSES_TAKEN]++;
        delay(1000);
        player.skill[school_classes[option-1].skill] += 100000;
        if (player.skill[school_classes[option-1].skill] > 9900000) player.skill[school_classes[option-1].skill] = 9900000;
        show_top(name,&menu);
        }
      }
    }
  } /* void school(char* name) */

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

#define STATUS_COUNT      12
#define STATUS_ETHER      11
#define STATUS_LINE        8
#define STATUS_BASELINE   (STATUS_LINE * 11)     /* outtext_xy()'s baseline for the line */
#define NO_STATUS_TILE    10

/* A status: its letter, the colour of its icon's art, its icon, and whether the player has it */
typedef struct {
  char  letter;
  Int32 color;
  Int32 icon;
  Int32 active;
  } STATUS_MARK;

/* The mode the status row was last drawn in */
static char status_mode_drawn;

static void read_statuses(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 ether;

  ether = player.effect[PE_ETHER];

  marks[0]  = (STATUS_MARK){ 'L', CLR_BROWN,      43, can_see() != 0 };
  marks[1]  = (STATUS_MARK){ 'S', CLR_LIGHTBLUE,  47, player.effect[PE_PROT] != 0 };
  marks[2]  = (STATUS_MARK){ 'P', CLR_WHITE,      46, player.effect[PE_POISON] != 0 };
  marks[3]  = (STATUS_MARK){ 'H', CLR_WHITE,      50, has_intrinsic(A_SPD) != 0 };
  marks[4]  = (STATUS_MARK){ 'G', CLR_YELLOW,     45, player.effect[PE_GIANT] != 0 };
  marks[5]  = (STATUS_MARK){ 'V', CLR_BROWN,     325, has_intrinsic(A_LEV) != 0 };
  marks[6]  = (STATUS_MARK){ 'F', CLR_LIGHTRED,   44, has_intrinsic(A_RFIR) != 0 };
  marks[7]  = (STATUS_MARK){ 'C', CLR_WHITE,     354, has_intrinsic(A_RCLD) != 0 };
  marks[8]  = (STATUS_MARK){ 'Z', CLR_YELLOW,    355, has_intrinsic(A_RELC) != 0 };
  marks[9]  = (STATUS_MARK){ 'A', CLR_PURPLE,    356, has_intrinsic(A_RDRK) != 0 };
  marks[10] = (STATUS_MARK){ 'M', CLR_WHITE,     357, has_intrinsic(A_RPSI) != 0 };

  if (ether > 10) marks[STATUS_ETHER] = (STATUS_MARK){ 'E', CLR_WHITE, 48, 1 };
    else marks[STATUS_ETHER] = (STATUS_MARK){ 'e', CLR_GRAY, 49, ether > 0 };
  }

static Int32 status_column(Int32 i) {
  return TEXT_GRID_WIDTH - 24 + 2 * i;
  }

static Int32 icon_width() {
  return (tile_width + 1) / 2;
  }

static Int32 icon_height() {
  return (tile_height + 1) / 2;
  }

/* An icon is centred on its letter's 8x12 cell */
static Int32 icon_left(Int32 i) {
  return status_column(i) * 8 + 4 - icon_width() / 2;
  }

static Int32 icon_top() {
  return STATUS_BASELINE - 10 + 6 - icon_height() / 2;
  }

/* The letters as 7.0.0 drew them: a space leaves the letter there */
static void show_grey_letters(STATUS_MARK marks[STATUS_COUNT]) {
  char  text[2];
  Int32 i;

  setcolor(7);
  for (i=0; i<STATUS_COUNT; i++) {
    text[0] = marks[i].active ? marks[i].letter : ' ';
    text[1] = 0;
    outtext_xy(status_column(i),STATUS_LINE,text);
    }
  }

static void show_colored_letters(STATUS_MARK marks[STATUS_COUNT]) {
  char  text[2];
  Int32 i;

  for (i=0; i<STATUS_COUNT; i++) {
    text[0] = marks[i].active ? marks[i].letter : ' ';
    text[1] = 0;
    setcolor(marks[i].color);
    outtextxyclr(status_column(i) * 8,STATUS_BASELINE,text);
    }
  }

static void show_status_icons(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 i;

  setfillstyle(1,0);
  for (i=0; i<STATUS_COUNT; i++) {
    bar(icon_left(i),icon_top(),icon_left(i) + icon_width(),icon_top() + icon_height());
    if (marks[i].active) put_half_image(icon_left(i),icon_top(),select_sprite(marks[i].icon));
    }
  }

/* Off, Color or Icons, as the Improved Status preference has it. A change of mode clears the row first: the icons
   cover more than a letter's cell, and the grey letters never clear what was there. */
static void show_status_row(STATUS_MARK marks[STATUS_COUNT]) {
  if (improved_status != status_mode_drawn) {
    setfillstyle(1,0);
    bar(icon_left(0),icon_top(),icon_left(STATUS_COUNT - 1) + icon_width(),icon_top() + icon_height());
    status_mode_drawn = improved_status;
    }

  switch (improved_status) {
    case 'C': show_colored_letters(marks); break;
    case 'I': show_status_icons(marks); break;
    default:  show_grey_letters(marks); break;
    }

  /* Callers draw on in the grey 7.0.0 left set */
  setcolor(7);
  }

/* The icons along the bottom of the screen. An ether icon stays until the screen is redrawn. */
static void show_status_tiles(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 i;

  for (i=0; i<STATUS_COUNT; i++) {
    if (i == STATUS_ETHER && !marks[i].active) continue;
    putimage(tile_width*i,screen_height-tile_height,select_sprite(marks[i].active ? marks[i].icon : NO_STATUS_TILE),0);
    }
  }

void show_time() {
  setfillstyle(1,0);
  bar(screen_width-130,12,screen_width,23);
  setcolor(14);
  sprintf(buffer,"%02d/%02d  %02d:%02d",player.month,player.day,player.hour,player.min);
  outtext_xy(TEXT_GRID_WIDTH - 15,2,buffer);
  }

void show_side() {
  char        buffer[80];
  STATUS_MARK marks[STATUS_COUNT];
  setfillstyle(1,0);
  bar(screen_width - 130,10,screen_width,100);
  settextstyle(0,0,1);
  if (player.effect[PE_HEROIC] < 0) {
    setcolor(10);
    outtext_xy(TEXT_GRID_WIDTH - 10,0,"Casual");
    }
  if (player.effect[PE_HEROIC] == 2) {
    setcolor(12);
    outtext_xy(TEXT_GRID_WIDTH - 10,0,"Heroic");
    }
  if (player.effect[PE_HEROIC] == 4) {
    setcolor(13);
    outtext_xy(TEXT_GRID_WIDTH - 10,0,"EPIC");
    }
  show_time();
  setcolor(14);
  outtext_xy(TEXT_GRID_WIDTH - 15,1,make_str(player.d_level));
  show_time();
  sprintf(buffer,"%d/%d    ",player.chp,get_mhp());
  outtext_xy(TEXT_GRID_WIDTH - 15,3,buffer);
  sprintf(buffer,"%d/%d    ",player.cmana,get_mmana());
  outtext_xy(TEXT_GRID_WIDTH - 15,4,buffer);
  outtext_xy(TEXT_GRID_WIDTH - 15,5,make_str(player.level));
  sprintf(buffer,"(%s)",make_str(player.exp));
  outtext_xy(TEXT_GRID_WIDTH - 12,5,buffer);
  if (player.health >= 101 && player.health <=999) {
    setcolor(CLR_LIGHTGREEN);
    outtext_xy(TEXT_GRID_WIDTH - 15,6,"Satiated");
    }
  if (player.health >= 75  && player.health <=100) {
    outtext_xy(TEXT_GRID_WIDTH - 15,6,"        ");
    }
  if (player.health >= 50 && player.health <=74) {
      setcolor(CLR_YELLOW);
      outtext_xy(TEXT_GRID_WIDTH - 15,6,"Hungry  ");
    }
  if (player.health >= 25 && player.health <=49) {
      setcolor(CLR_LIGHTRED);
      outtext_xy(TEXT_GRID_WIDTH - 15,6,"Starving");
    }
  if (player.health >= 1 && player.health <=24) {
      setcolor(CLR_LIGHTPURPLE);
       outtext_xy(TEXT_GRID_WIDTH - 15,6,"Faint   ");
    }
  if (player.health <= 0) {
    outtext_xy(TEXT_GRID_WIDTH - 15,6,"DEAD!!");
    }
  setcolor(CLR_YELLOW);
/*  show_sprite(22,2,((player.fire != 0)?44:10)); */
/*  show_sprite(20,2,((player.giant != 0)?45:10)); */
/*  show_sprite(17,2,((player.haste != 0)?50:10)); */
/*  show_sprite(19,2,((player.prot != 0)?47:10)); */
/*  show_sprite(23,2,((player.light != 0)?43:10)); */
/*  show_sprite(18,2,((player.poison != 0)?46:10)); */
/*
  putimage(statusCol,statusLine,select_sprite((player.effect[PE_LIGHT] != 0)?43:10),0);
  putimage(statusCol+35,statusLine,select_sprite((player.effect[PE_PROT] != 0)?47:10),0);
  putimage(statusCol+70,statusLine,select_sprite((player.effect[PE_POISON] != 0)?46:10),0);
  putimage(statusCol+105,statusLine,select_sprite((player.effect[PE_HASTE] != 0)?50:10),0);
  putimage(statusCol+140,statusLine,select_sprite((player.effect[PE_GIANT] != 0)?45:10),0);
  putimage(statusCol+175,statusLine,select_sprite((player.effect[PE_FIRE] != 0)?44:10),0);
  putimage(statusCol+210,statusLine,select_sprite((player.effect[PE_LEV] != 0)?325:10),0);
*/
  read_statuses(marks);
  show_status_row(marks);
  /*
  if (player.effect[PE_ETHER] > 10) show_sprite(21,2,49);
    else if (player.effect[PE_ETHER] > 0) show_sprite(21,2,48);
      else show_sprite(21,2,10);
      */
  show_status_tiles(marks);
  }


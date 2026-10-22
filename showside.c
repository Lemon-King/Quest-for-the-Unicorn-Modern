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

#define STATUS_ETHER      11
#define STATUS_LINE        8
#define STATUS_BASELINE   (STATUS_LINE * 11)     /* outtext_xy()'s baseline for the line */
#define NO_STATUS_TILE    10

#define LEVEL_LINE         5
#define HUNGER_LINE        6
#define XP_BAR_TOP        (LEVEL_LINE * 11 + 2)  /* two pixels under the Level line's letters */
#define XP_BAR_HEIGHT      4
#define HUNGER_DROP        6                     /* the hunger line moves down past the XP bar */
#define EXP_STEP_MARGIN    1.01                  /* a saved exp_next can be off its step by rounding */

/* A status: its letter, its colour, its icon, and whether the player has it */
typedef struct {
  char  letter;
  Int32 color;
  Int32 icon;
  Int32 active;
  } STATUS_MARK;

/* The mode the status row was last drawn in */
static char status_mode_drawn;

/* Each status's name and what it does, in the row's order */
static const char* status_names[STATUS_COUNT] = {
  "Light", "Shielded", "Poisoned", "Hasted", "Giant Strength", "Levitating", "Resist Fire", "Resist Cold",
  "Resist Electricity", "Resist Arcane", "Resist Mind", "Ethereal"
  };

static const char* status_effects[STATUS_COUNT] = {
  "Darkness bothers you no longer",
  "Protection from spells",
  "Poison lingers in your veins",
  "Your movements are swift",
  "You have the strength of a giant",
  "You float just off the ground",
  "You are protected from Fire",
  "You are protected from Cold",
  "You are protected from Electricity",
  "You are protected from Arcane",
  "You are resistant to Mind attacks",
  "Your body is translucent like a ghost"
  };

/* The effect that times each status, -1 for none, and whether it counts down every turn in towns and dungeons rather
   than every 1 + level/6 turns */
static const Int32 status_timers[STATUS_COUNT][2] = {
  { PE_LIGHT, 1 }, { PE_PROT, 0 }, { PE_POISON, 1 }, { PE_HASTE, 0 }, { PE_GIANT, 0 }, { PE_LEV, 1 },
  { PE_FIRE, 0 }, { PE_COLD, 0 }, { PE_ELEC, 0 }, { PE_ARCN, 0 }, { PE_MIND, 0 }, { PE_ETHER, 1 }
  };

static void read_statuses(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 ether;

  ether = player.effect[PE_ETHER];

  marks[0]  = (STATUS_MARK){ 'L', CLR_BROWN,        43, can_see() != 0 };
  marks[1]  = (STATUS_MARK){ 'S', CLR_LIGHTBLUE,    47, player.effect[PE_PROT] != 0 };
  marks[2]  = (STATUS_MARK){ 'P', CLR_GREEN,        46, player.effect[PE_POISON] != 0 };
  marks[3]  = (STATUS_MARK){ 'H', CLR_WHITE,        50, has_intrinsic(A_SPD) != 0 };
  marks[4]  = (STATUS_MARK){ 'G', CLR_YELLOW,       45, player.effect[PE_GIANT] != 0 };
  marks[5]  = (STATUS_MARK){ 'V', CLR_WHITE,       325, has_intrinsic(A_LEV) != 0 };
  marks[6]  = (STATUS_MARK){ 'F', CLR_LIGHTRED,     44, has_intrinsic(A_RFIR) != 0 };
  marks[7]  = (STATUS_MARK){ 'C', CLR_LIGHTBLUE,   354, has_intrinsic(A_RCLD) != 0 };
  marks[8]  = (STATUS_MARK){ 'Z', CLR_CYAN,        355, has_intrinsic(A_RELC) != 0 };
  marks[9]  = (STATUS_MARK){ 'A', CLR_PURPLE,      356, has_intrinsic(A_RDRK) != 0 };
  marks[10] = (STATUS_MARK){ 'M', CLR_LIGHTPURPLE, 357, has_intrinsic(A_RPSI) != 0 };

  if (ether > 10) marks[STATUS_ETHER] = (STATUS_MARK){ 'E', CLR_WHITE, 48, 1 };
    else marks[STATUS_ETHER] = (STATUS_MARK){ 'e', CLR_GRAY, 49, ether > 0 };
  }

static Int32 status_column(Int32 i) {
  return TEXT_GRID_WIDTH - 24 + 2 * i;
  }

/* Where you have light from, in can_see()'s order */
static void light_source(char* text) {
  if (in_upland == 'Y') strcpy(text,"Always, on the overworld");
  else if (player.effect[PE_LIGHT] > 0) sprintf(text,"Your light lasts %d more turns",player.effect[PE_LIGHT]);
  else if (player.effect[PE_LIGHT] < 0) strcpy(text,"Your light never goes out");
  else if (player.in_city == 'Y') strcpy(text,"Always, in towns");
  else strcpy(text,"By infravision");
  }

/* How long a status you have lasts, or where it comes from */
static void status_lasting(Int32 i,char* text) {
  Int32 left;
  Int32 turns;

  left = player.effect[status_timers[i][0]];
  turns = status_timers[i][1] ? left : left * (1 + player.level / 6);

  if (i == 0) light_source(text);
  else if (i == 2) sprintf(text,"Takes %d damage a turn",(left / 10 > 1) ? left / 10 : 1);
  else if (left <= 0) strcpy(text,"From your gear, race or class");

  /* Without the Mind Shield change nothing counts a Mind Shield down */
  else if (i == 10 && change_mind_shield != 'Y') strcpy(text,"Does not wear off");
  else if (status_timers[i][1]) sprintf(text,"Lasts %d more turns",turns);
  else sprintf(text,"Lasts about %d more turns",turns);
  }

void describe_status(Int32 i,STATUS_NOTE* note) {
  STATUS_MARK marks[STATUS_COUNT];

  read_statuses(marks);
  strcpy(note->name,status_names[i]);
  strcpy(note->effect,status_effects[i]);
  note->color = marks[i].color;
  note->active = marks[i].active;
  note->x = status_column(i) * 8;

  if (note->active) status_lasting(i,note->lasting);
    else strcpy(note->lasting,"Not active");
  }

/* An icon is centred on its letter's 8x12 cell */
static Int32 icon_left(Int32 i) {
  return status_column(i) * 8 + 4 - half_tile_width() / 2;
  }

static Int32 icon_top() {
  return STATUS_BASELINE - 10 + 6 - half_tile_height() / 2;
  }

/* Each letter's cell is cleared first, so a status that ends leaves no letter behind */
static void show_letters(STATUS_MARK marks[STATUS_COUNT], Int32 colored) {
  char  text[2];
  Int32 i;

  for (i=0; i<STATUS_COUNT; i++) {
    text[0] = marks[i].active ? marks[i].letter : ' ';
    text[1] = 0;
    setcolor(colored ? marks[i].color : 7);
    outtextxyclr(status_column(i) * 8,STATUS_BASELINE,text);
    }
  }

static void show_status_icons(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 i;

  setfillstyle(1,0);
  for (i=0; i<STATUS_COUNT; i++) {
    bar(icon_left(i),icon_top(),icon_left(i) + half_tile_width(),icon_top() + half_tile_height());
    if (marks[i].active) put_half_image(icon_left(i),icon_top(),select_sprite(marks[i].icon));
    }
  }

/* Off, Color or Icons, as the Improved Status preference has it. A change of mode clears the row first, since the
   icons cover more than a letter's cell. */
static void show_status_row(STATUS_MARK marks[STATUS_COUNT]) {
  if (improved_status != status_mode_drawn) {
    setfillstyle(1,0);
    bar(icon_left(0),icon_top(),icon_left(STATUS_COUNT - 1) + half_tile_width(),icon_top() + half_tile_height());
    status_mode_drawn = improved_status;
    }

  switch (improved_status) {
    case 'C': show_letters(marks,1); break;
    case 'I': show_status_icons(marks); break;
    default:  show_letters(marks,0); break;
    }

  /* Callers draw on in the grey 7.0.0 left set */
  setcolor(7);
  }

/* The icons along the bottom of the screen, which Icons mode gives to the map */
static void show_status_tiles(STATUS_MARK marks[STATUS_COUNT]) {
  Int32 i;

  if (!has_bottom_strip()) return;

  for (i=0; i<STATUS_COUNT; i++)
    putimage(tile_width*i,screen_height-tile_height,select_sprite(marks[i].active ? marks[i].icon : NO_STATUS_TILE),0);
  }

void show_time() {
  setfillstyle(1,0);
  bar(screen_width-130,12,screen_width,23);
  setcolor(14);
  sprintf(buffer,"%02d/%02d  %02d:%02d",player.month,player.day,player.hour,player.min);
  outtext_xy(TEXT_GRID_WIDTH - 15,2,buffer);
  }

#define NUM_HUNGER_BANDS   5

/* A hunger band, from its least health */
typedef struct {
  Int32  least;
  HUNGER hunger;
  } HUNGER_BAND;

/* From the most fed down; below the last you are dead */
static const HUNGER_BAND hunger_bands[NUM_HUNGER_BANDS] = {
  { 101, { "Satiated", CLR_LIGHTGREEN,  1 } },
  {  75, { "Healthy",  CLR_GREEN,       0 } },
  {  50, { "Hungry",   CLR_YELLOW,      1 } },
  {  25, { "Starving", CLR_LIGHTRED,    1 } },
  {   1, { "Faint",    CLR_LIGHTPURPLE, 1 } }
  };

static const HUNGER dead = { "DEAD!!", CLR_YELLOW, 1 };

HUNGER hunger_status(void) {
  Int32 i;

  for (i=0; i<NUM_HUNGER_BANDS; i++)
    if (player.health >= hunger_bands[i].least) return hunger_bands[i].hunger;

  return dead;
  }

/* The experience the current level began at: the step before exp_next in the class's steps, which are at least 20%
   apart */
static double level_start_exp(void) {
  double start;
  double next;

  start = 0;
  next = classes[player.class-1].first_exp;
  while (next * EXP_STEP_MARGIN < player.exp_next) {
    start = next;
    next = next_level_exp(next);
    }

  return start;
  }

/* How far through the current level the player is, from 0 to 1 */
static double level_progress(void) {
  double start;
  double progress;

  start = level_start_exp();
  if (player.exp_next <= start) return 0;

  progress = (player.exp - start) / (player.exp_next - start);
  if (progress < 0) return 0;
  if (progress > 1) return 1;

  return progress;
  }

/* The XP bar and the hunger line under it span the panel's rows, as far from the screen's right edge as from the frame
   on their left */
static Int32 row_left(void) {
  return status_column(0) * 8;
  }

static Int32 row_right(void) {
  return screen_width - (row_left() - side_panel_left());
  }

/* With Improved Interface, a bar under Level, yellow on grey for the level's progress; without it, its place is
   cleared */
static void show_xp_bar(void) {
  Int32 left;
  Int32 right;
  Int32 filled;

  left = row_left();
  right = row_right();

  if (improved_interface != 'Y') {
    setfillstyle(1,CLR_BLACK);
    bar(left,XP_BAR_TOP,right,XP_BAR_TOP + XP_BAR_HEIGHT);
    return;
    }

  filled = left + (Int32)(level_progress() * (right - left));

  setfillstyle(1,CLR_GRAY);
  bar(filled,XP_BAR_TOP,right,XP_BAR_TOP + XP_BAR_HEIGHT);
  setfillstyle(1,CLR_YELLOW);
  bar(left,XP_BAR_TOP,filled,XP_BAR_TOP + XP_BAR_HEIGHT);
  }

/* With Improved Interface the hunger line is centred under the XP bar. Its line there is cleared either way, so a
   shorter name, or turning the preference off, leaves nothing behind. */
static void show_hunger(void) {
  HUNGER hunger;
  Int32  baseline;
  Int32  x;

  baseline = HUNGER_LINE * 11 + HUNGER_DROP;
  setfillstyle(1,CLR_BLACK);
  bar(row_left(),baseline - 10,row_right(),baseline + 2);

  hunger = hunger_status();
  if (!hunger.on_panel) return;

  setcolor(hunger.color);
  if (improved_interface == 'Y') {
    x = (row_left() + row_right() - 8 * (Int32)strlen(hunger.name)) / 2;
    outtextxy(x,baseline,hunger.name);
    }
  else outtext_xy(TEXT_GRID_WIDTH - 15,HUNGER_LINE,hunger.name);
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
  outtext_xy(TEXT_GRID_WIDTH - 15,LEVEL_LINE,make_str(player.level));
  sprintf(buffer,"(%s)",make_str(player.exp));
  outtext_xy(TEXT_GRID_WIDTH - 12,LEVEL_LINE,buffer);
  show_xp_bar();
  show_hunger();
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


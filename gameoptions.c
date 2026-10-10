/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Game options: the new-game setup screen and the in-game options screen. */

#include "header.h"

#define OPT_MODE        1
#define OPT_SWITCH      2
#define OPT_NUMBER      3

#define NUM_MODES       4
#define NUM_RULES       6
#define NUM_OPTIONS    14

#define RULES_LINE      3
#define PREFS_LINE     11
#define HELP_LINE      22
#define LABEL_COLUMN    9
#define VALUE_COLUMN   34

typedef struct {
  char   label[24];
  char   help[80];
  Int32  kind;
  char   rule;
  char*  flag;
  char   on;
  char   off;
  Int32* number;
  Int32  min;
  Int32  max;
  Int32  step;
  Int32  initial;
  } OPTION_ROW;

static Int32 mode_values[NUM_MODES] = { 0, 2, 4, -1 };
static char* mode_names[NUM_MODES]  = { "Normal", "Heroic", "Epic", "Casual" };

/* The first NUM_RULES rows are rules, fixed once the character is created. */
static OPTION_ROW option_rows[NUM_OPTIONS] = {
  { .label = "Game mode",
    .help  = "Heroic: monsters x2.  Epic: monsters x4.  Casual: no permadeath or scores",
    .kind  = OPT_MODE,   .rule = 'Y', .number = &heroicMode },
  { .label = "Dungeon depth",
    .help  = "Number of levels in each dungeon, from 2 to 10",
    .kind  = OPT_NUMBER, .rule = 'Y', .number = &bottom, .min = 2, .max = 10, .step = 1 },
  { .label = "Monster count",
    .help  = "Monsters or monster groups that can spawn on a level, from 10 to 60",
    .kind  = OPT_NUMBER, .rule = 'Y', .number = &monster_count, .min = 10, .max = 60, .step = 5 },
  { .label = "Arena combat",
    .help  = "Multi-monster arena combat on higher levels; when off, only bosses",
    .kind  = OPT_SWITCH, .rule = 'Y', .flag = &no_arena, .on = 'N', .off = 'Y' },
  { .label = "Dungeon scaling",
    .help  = "Monsters in dungeons scale to your level",
    .kind  = OPT_SWITCH, .rule = 'Y', .flag = &dungeon_scaling, .on = 'Y', .off = 'N' },
  { .label = "Full party combat",
    .help  = "Monsters and party members take full damage from each other",
    .kind  = OPT_SWITCH, .rule = 'Y', .flag = &party_combat_mode, .on = 'F', .off = 'R' },
  { .label = "Auto target",
    .help  = "Target the nearest monster automatically when aiming",
    .kind  = OPT_SWITCH, .rule = 'N', .flag = &auto_target, .on = 'Y', .off = 'N' },
  { .label = "Auto pickup gold",
    .help  = "Pick up gold automatically when you walk over it",
    .kind  = OPT_SWITCH, .rule = 'N', .flag = &auto_pickup_gold, .on = 'Y', .off = 'N' },
  { .label = "Auto pickup torches",
    .help  = "Pick up torches automatically when you walk over them",
    .kind  = OPT_SWITCH, .rule = 'N', .flag = &auto_pickup_torch, .on = 'Y', .off = 'N' },
  { .label = "Auto open doors",
    .help  = "Open a door automatically when you bump into it",
    .kind  = OPT_SWITCH, .rule = 'N', .flag = &auto_open_doors, .on = 'Y', .off = 'N' },
  { .label = "Auto search walls",
    .help  = "Search automatically when you bump into a wall",
    .kind  = OPT_SWITCH, .rule = 'N', .flag = &auto_bump_walls, .on = 'Y', .off = 'N' },
  { .label = "Ranged delay (ms)",
    .help  = "Delay for the flight of ranged attacks",
    .kind  = OPT_NUMBER, .rule = 'N', .number = &delay_ranged, .min = 0, .max = 1000, .step = 5 },
  { .label = "Hit delay (ms)",
    .help  = "Delay for the successful hit effect",
    .kind  = OPT_NUMBER, .rule = 'N', .number = &delay_hit, .min = 0, .max = 1000, .step = 25 },
  { .label = "Trap delay (ms)",
    .help  = "Delay for showing trap effects",
    .kind  = OPT_NUMBER, .rule = 'N', .number = &delay_trap, .min = 0, .max = 1000, .step = 25 }
  };

static Int32 mode_index(Int32 mode) {
  Int32 i;
  for (i=0; i<NUM_MODES; i++)
    if (mode_values[i] == mode) return i;
  return 0;
  }

static Int32 is_editable(Int32 i, char rules_editable) {
  return (option_rows[i].rule != 'Y' || rules_editable == 'Y');
  }

static Int32 option_line(Int32 i) {
  if (i < NUM_RULES) return RULES_LINE + 1 + i;
  return PREFS_LINE + 1 + i - NUM_RULES;
  }

static Int32 next_option(Int32 i, Int32 dir, char rules_editable) {
  do {
    i += dir;
    if (i < 0) i = NUM_OPTIONS - 1;
    if (i >= NUM_OPTIONS) i = 0;
    } while (!is_editable(i, rules_editable));
  return i;
  }

static void format_value(OPTION_ROW* row, char* text) {
  switch (row->kind) {
    case OPT_MODE:   sprintf(text, "%-8s", mode_names[mode_index(*row->number)]); break;
    case OPT_SWITCH: sprintf(text, "%-8s", (*row->flag == row->on) ? "On" : "Off"); break;
    case OPT_NUMBER: sprintf(text, "%-8d", *row->number); break;
    }
  }

static void change_option(OPTION_ROW* row, Int32 dir) {
  switch (row->kind) {
    case OPT_MODE:
         *row->number = mode_values[(mode_index(*row->number) + dir + NUM_MODES) % NUM_MODES];
         break;
    case OPT_SWITCH:
         *row->flag = (*row->flag == row->on) ? row->off : row->on;
         break;
    case OPT_NUMBER:
         *row->number += dir * row->step;
         if (*row->number < row->min) *row->number = row->min;
         if (*row->number > row->max) *row->number = row->max;
         break;
    }
  }

static void draw_option(Int32 i, char selected, char rules_editable) {
  char  text[16];
  Int32 line;
  line = option_line(i);

  textcolor(15);
  gotoxy(5, line); txt_write((selected == 'Y') ? "-->" : "   ");

  textcolor(is_editable(i, rules_editable) ? 2 : 8);
  gotoxy(LABEL_COLUMN, line); txt_write(option_rows[i].label);

  textcolor(is_editable(i, rules_editable) ? 14 : 8);
  format_value(&option_rows[i], text);
  gotoxy(VALUE_COLUMN, line); txt_write(text);
  }

static void draw_help(Int32 i) {
  char text[80];
  sprintf(text, "%-76s", option_rows[i].help);
  textcolor(7);
  gotoxy(3, HELP_LINE); txt_write(text);
  }

static void draw_screen(char* title, char rules_editable) {
  Int32 i;
  textmode(3);
  textcolor(12);
  gotoxy(41 - strlen(title) / 2, 1); txt_write(title);

  textcolor(11);
  gotoxy(3, RULES_LINE); txt_write("Character rules (fixed once the character is created)");
  gotoxy(3, PREFS_LINE); txt_write("Preferences (press O during play to change them)");
  for (i=0; i<NUM_OPTIONS; i++) draw_option(i, 'N', rules_editable);

  textcolor(3);
  gotoxy(20, 24); txt_write("(8/2) Select   (4/6) Change   (ENTER) Done");
  }

static void edit_options(char* title, char rules_editable) {
  Int32 i;
  char  key;
  draw_screen(title, rules_editable);
  i = next_option(-1, 1, rules_editable);

  key = 0;
  while (key != 13 && key != 27) {
    draw_option(i, 'Y', rules_editable);
    draw_help(i);
    key = inkey();
    draw_option(i, 'N', rules_editable);
    if (key == '8') i = next_option(i, -1, rules_editable);
    if (key == '2') i = next_option(i, 1, rules_editable);
    if (key == '4' || key == '-') change_option(&option_rows[i], -1);
    if (key == '6' || key == '+' || key == '=') change_option(&option_rows[i], 1);
    }
  }

/* Keeps the values from unicorn.cfg and the command line, which pre-fill every new game. */
void remember_option_defaults() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) {
    if (option_rows[i].kind == OPT_SWITCH) option_rows[i].initial = *option_rows[i].flag;
      else option_rows[i].initial = *option_rows[i].number;
    }
  }

static void restore_option_defaults() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) {
    if (option_rows[i].kind == OPT_SWITCH) *option_rows[i].flag = option_rows[i].initial;
      else *option_rows[i].number = option_rows[i].initial;
    }
  }

/* Sets the session's rule options from the character's player.effect[] entries. */
void read_character_rules() {
  heroicMode = player.effect[PE_HEROIC];
  bottom = player.effect[PE_DEPTH];
  monster_count = player.effect[PE_MCOUNT];
  no_arena = (player.effect[PE_NOARENA] != 0) ? 'Y' : 'N';
  }

void game_setup() {
  restore_option_defaults();
  edit_options("New Game Options", 'Y');
  }

void game_options() {
  read_character_rules();
  edit_options("Game Options", 'N');
  }

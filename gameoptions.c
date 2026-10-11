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
#define NUM_RULES       7
#define NUM_PREFS      10
#define NUM_CHANGES     0
#define FIRST_CHANGE   (NUM_RULES + NUM_PREFS)
#define NUM_OPTIONS    (FIRST_CHANGE + NUM_CHANGES)

#define RULES_LINE      2
#define PREFS_LINE     11
#define CHANGES_LINE    2
#define HELP_LINE      23
#define LEFT_COLUMN     3
#define RIGHT_COLUMN   42
#define ARROW_INDENT    2
#define LABEL_INDENT    6
#define VALUE_INDENT   31

/* units follows a number's value; config names the unicorn.cfg key of a number kept there instead of in the save;
   apply puts a new value into effect; key names a gameplay change in the save */
typedef struct {
  char   label[24];
  char   help[80];
  char   units[4];
  Int32  kind;
  char*  flag;
  char   on;
  char   off;
  char   casual;
  Int32* number;
  Int32  min;
  Int32  max;
  Int32  step;
  Int32  initial;
  char*  config;
  void   (*apply)();
  char*  key;
  } OPTION_ROW;

static Int32 mode_values[NUM_MODES] = { 0, 2, 4, -1 };
static char* mode_names[NUM_MODES]  = { "Normal", "Heroic", "Epic", "Casual" };

/* NUM_RULES rules, NUM_PREFS preferences, then NUM_CHANGES gameplay changes. Rules and gameplay changes are fixed once
   the character is created. A switch with a casual value is held at it in Casual mode. A gameplay change is a switch
   with a key: On plays with the change, Off plays as the original game. */
static OPTION_ROW option_rows[NUM_OPTIONS] = {
  { .label = "Game Mode",
    .help  = "Heroic: monsters x2 | Epic: monsters x4 | Casual: no permadeath or scores",
    .kind  = OPT_MODE,   .number = &heroicMode },
  { .label = "Permadeath",
    .help  = "Death is final",
    .kind  = OPT_SWITCH, .flag = &permadeath, .on = 'Y', .off = 'N', .casual = 'N' },
  { .label = "Dungeon Depth",
    .help  = "Number of levels in each dungeon, from 2 to 10",
    .kind  = OPT_NUMBER, .number = &bottom, .min = 2, .max = 10, .step = 1 },
  { .label = "Monster Count",
    .help  = "Spawn size for monsters or monster groups, from 10 to 60",
    .kind  = OPT_NUMBER, .number = &monster_count, .min = 10, .max = 60, .step = 5 },
  { .label = "Arena Combat",
    .help  = "On: Multi-monster arena combat | Off: only bosses",
    .kind  = OPT_SWITCH, .flag = &no_arena, .on = 'N', .off = 'Y' },
  { .label = "Dungeon Scaling",
    .help  = "Monsters in dungeons scale to your level",
    .kind  = OPT_SWITCH, .flag = &dungeon_scaling, .on = 'Y', .off = 'N' },
  { .label = "Full Party Combat",
    .help  = "Monsters and party members take full damage",
    .kind  = OPT_SWITCH, .flag = &party_combat_mode, .on = 'F', .off = 'R' },
  { .label = "Auto-Target",
    .help  = "Target the nearest monster automatically",
    .kind  = OPT_SWITCH, .flag = &auto_target, .on = 'Y', .off = 'N' },
  { .label = "Auto-Pickup Gold",
    .help  = "Pick up gold automatically",
    .kind  = OPT_SWITCH, .flag = &auto_pickup_gold, .on = 'Y', .off = 'N' },
  { .label = "Auto-Pickup Torches",
    .help  = "Pick up torches automatically",
    .kind  = OPT_SWITCH, .flag = &auto_pickup_torch, .on = 'Y', .off = 'N' },
  { .label = "Auto-Open Doors",
    .help  = "Doors open automatically",
    .kind  = OPT_SWITCH, .flag = &auto_open_doors, .on = 'Y', .off = 'N' },
  { .label = "Auto-Search Walls",
    .help  = "Search walls automatically",
    .kind  = OPT_SWITCH, .flag = &auto_bump_walls, .on = 'Y', .off = 'N' },
  { .label = "Ranged Delay (ms)",
    .help  = "Ranged attack flight delay",
    .kind  = OPT_NUMBER, .number = &delay_ranged, .min = 0, .max = 1000, .step = 5 },
  { .label = "Hit Delay (ms)",
    .help  = "Successful hit effect delay",
    .kind  = OPT_NUMBER, .number = &delay_hit, .min = 0, .max = 1000, .step = 25 },
  { .label = "Trap Delay (ms)",
    .help  = "Trap effects delay",
    .kind  = OPT_NUMBER, .number = &delay_trap, .min = 0, .max = 1000, .step = 25 },
  { .label = "Enhanced Sprites",
    .help  = "Improves tile display for Sprites",
    .kind  = OPT_SWITCH, .flag = &enhanced_sprites, .on = 'Y', .off = 'N' },
  { .label = "Music Volume",
    .help  = "Music Volume from 0% to 100%", .units = "%",
    .kind  = OPT_NUMBER, .number = &music_volume, .min = 0, .max = 100, .step = 10,
    .config = "MusicVolume", .apply = set_music_volume }
  };

static Int32 mode_index(Int32 mode) {
  Int32 i;
  for (i=0; i<NUM_MODES; i++)
    if (mode_values[i] == mode) return i;
  return 0;
  }

char* mode_name(Int32 mode) {
  return mode_names[mode_index(mode)];
  }

static Int32 is_held_by_mode(OPTION_ROW* row) {
  return (row->casual != 0 && heroicMode < 0);
  }

static Int32 is_gameplay_change(Int32 i) {
  return (i >= FIRST_CHANGE);
  }

static Int32 is_new_game_only(Int32 i) {
  return (i < NUM_RULES || is_gameplay_change(i));
  }

static Int32 is_editable(Int32 i, char rules_editable) {
  if (is_held_by_mode(&option_rows[i])) return 0;
  return (!is_new_game_only(i) || rules_editable == 'Y');
  }

static Int32 option_line(Int32 i) {
  if (i < NUM_RULES) return RULES_LINE + 1 + i;
  if (i < FIRST_CHANGE) return PREFS_LINE + 1 + i - NUM_RULES;
  return CHANGES_LINE + 1 + i - FIRST_CHANGE;
  }

static Int32 option_column(Int32 i) {
  return is_gameplay_change(i) ? RIGHT_COLUMN : LEFT_COLUMN;
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
  char flag;
  char number[16];
  switch (row->kind) {
    case OPT_MODE:   sprintf(text, "%-8s", mode_name(*row->number)); break;
    case OPT_SWITCH: flag = is_held_by_mode(row) ? row->casual : *row->flag;
                     sprintf(text, "%-8s", (flag == row->on) ? "On" : "Off");
                     break;
    case OPT_NUMBER: sprintf(number, "%d%s", *row->number, row->units);
                     sprintf(text, "%-8s", number);
                     break;
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

  if (row->apply != NULL) row->apply();
  if (row->config != NULL) save_config_value("unicorn",row->config,*row->number);
  }

static void draw_option(Int32 i, char selected, char rules_editable) {
  char  text[16];
  Int32 line;
  Int32 column;
  line = option_line(i);
  column = option_column(i);

  textcolor(15);
  gotoxy(column + ARROW_INDENT, line); txt_write((selected == 'Y') ? "-->" : "   ");

  textcolor(is_editable(i, rules_editable) ? 2 : 8);
  gotoxy(column + LABEL_INDENT, line); txt_write(option_rows[i].label);

  textcolor(is_editable(i, rules_editable) ? 14 : 8);
  format_value(&option_rows[i], text);
  gotoxy(column + VALUE_INDENT, line); txt_write(text);
  }

static void draw_options(char rules_editable) {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) draw_option(i, 'N', rules_editable);
  }

static void draw_help(Int32 i) {
  char text[80];
  sprintf(text, "%-76s", option_rows[i].help);
  textcolor(7);
  gotoxy(3, HELP_LINE); txt_write(text);
  }

static void draw_screen(char* title, char rules_editable) {
  textmode(3);
  textcolor(12);
  gotoxy(41 - strlen(title) / 2, 1); txt_write(title);

  textcolor(11);
  gotoxy(LEFT_COLUMN, RULES_LINE); txt_write("Game Rules");
  gotoxy(LEFT_COLUMN, PREFS_LINE); txt_write("Preferences");
  gotoxy(RIGHT_COLUMN, CHANGES_LINE); txt_write("Gameplay Changes");
  draw_options(rules_editable);

  textcolor(3);
  gotoxy(20, 24); txt_write("(8/2) Select   (4/6) Change   (ENTER) Done");
  }

static void edit_options(char* title, char rules_editable) {
  Int32 i;
  Int32 dir;
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

    dir = 0;
    if (key == '4' || key == '-') dir = -1;
    if (key == '6' || key == '+' || key == '=') dir = 1;
    if (dir != 0) {
      change_option(&option_rows[i], dir);
      draw_options(rules_editable);
      }
    }
  }

/* Sets the switches Casual mode holds to their casual values */
static void apply_mode_holds() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++)
    if (is_held_by_mode(&option_rows[i])) *option_rows[i].flag = option_rows[i].casual;
  }

/* Keeps the values from unicorn.cfg and the command line, which pre-fill every new game. Gameplay changes start On. */
void remember_option_defaults() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) {
    if (is_gameplay_change(i)) option_rows[i].initial = option_rows[i].on;
      else if (option_rows[i].kind == OPT_SWITCH) option_rows[i].initial = *option_rows[i].flag;
      else option_rows[i].initial = *option_rows[i].number;
    }
  }

/* Rows kept in unicorn.cfg are saved as they change, so they keep their value. */
static void restore_option_defaults() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) {
    if (option_rows[i].config != NULL) continue;
    if (option_rows[i].kind == OPT_SWITCH) *option_rows[i].flag = option_rows[i].initial;
      else *option_rows[i].number = option_rows[i].initial;
    }
  }

/* A save from before a gameplay change plays as the original game. */
void clear_gameplay_changes() {
  Int32 i;
  for (i=FIRST_CHANGE; i<NUM_OPTIONS; i++) *option_rows[i].flag = option_rows[i].off;
  }

/* Writes each gameplay change into the save's Options section as "<key> <value>". */
void save_gameplay_changes(FILE* file) {
  char  line[64];
  Int32 i;
  for (i=FIRST_CHANGE; i<NUM_OPTIONS; i++) {
    sprintf(line, "  %s %c", option_rows[i].key, *option_rows[i].flag);
    write_line(file, line);
    }
  }

/* Reads one line of the save's Options section, if it names a gameplay change. */
void load_gameplay_change(char* line) {
  Int32 i;
  for (i=FIRST_CHANGE; i<NUM_OPTIONS; i++)
    if (match(line, option_rows[i].key) && line[strlen(option_rows[i].key)] == ' ')
      *option_rows[i].flag = *next_word(line);
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
  apply_mode_holds();
  }

void game_options() {
  read_character_rules();
  edit_options("Game Options", 'N');
  }

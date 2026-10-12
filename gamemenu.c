/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The Tab Menu, for the Tab Menu preference: in the box that shows the log, the commands for where you stand, then
   the common ones, then Save and Load. A command types its keys into the game, as if pressed. */

#include "header.h"

#include <ctype.h>

#define MAX_COMMANDS    48
#define MAX_LINES      (MAX_COMMANDS * 4)
#define LABEL_WIDTH     20
#define LABEL_LENGTH    72
#define INDENT           1     /* a label's later lines, under its first */
#define NAME_LENGTH     20     /* of a save named with Save as New */
#define PANEL_LEFT     215     /* from the right edge, where the log's lines start */
#define PANEL_TOP      100
#define MORE_COLUMN     19
#define KEY_COLUMN      24     /* a space after the label; the scroll bar takes column 26 */

#define CONTEXT_COLOR   10
#define COMMON_COLOR    14
#define HEADING_COLOR   11

#define DO_KEYS          0
#define DO_SAVE          1
#define DO_LOAD          2

/* A command: what it says, the keys it types (the first returned, the rest queued), or Save or Load. An item's name
   in its label, from name_start on, shows in its rarity colour, name_color; 0 for none. */
typedef struct {
  char  label[LABEL_LENGTH];
  char  keys[4];
  Int32 action;
  Int32 color;
  Int32 enabled;
  Int32 name_start;
  Int32 name_color;
  } COMMAND;

/* A line of the menu: a command's label wraps onto as many as it needs; offset is where in the label it starts */
typedef struct {
  Int32 command;
  Int32 first;
  Int32 offset;
  char  text[LABEL_WIDTH + 1];
  } COMMAND_LINE;

typedef struct {
  COMMAND      list[MAX_COMMANDS];
  Int32        count;
  COMMAND_LINE lines[MAX_LINES];
  Int32        line_count;
  } COMMANDS;

/* The eight squares around you, clockwise from north, with the key that steps there */
static const Int32 step_x[8]   = { 0, 1, 1, 1, 0, -1, -1, -1 };
static const Int32 step_y[8]   = { -1, -1, 0, 1, 1, 1, 0, -1 };
static const char  step_key[8] = { '8', '9', '6', '3', '2', '1', '4', '7' };

static char* save_choices[3] = { "Overwrite", "Save as New", "Cancel" };
static char* overwrite_choices[2] = { "Yes", "No" };

static void add_line(COMMANDS* commands,Int32 first,char* text,Int32 offset) {
  COMMAND_LINE* line;

  if (commands->line_count == MAX_LINES) return;

  line = &commands->lines[commands->line_count++];
  line->command = commands->count - 1;
  line->first = first;
  line->offset = offset;
  sprintf(line->text,"%*s%s",first ? 0 : INDENT,"",text);
  }

/* Breaks the newest command's label into lines at its spaces; a word too long for a line is cut */
static void wrap_label(COMMANDS* commands) {
  char  words[LABEL_LENGTH];
  char  line[LABEL_WIDTH + 1];
  char* word;
  Int32 first;
  Int32 width;
  Int32 length;
  Int32 start;

  strcpy(words,commands->list[commands->count - 1].label);
  strcpy(line,"");
  first = 1;
  width = LABEL_WIDTH;
  start = 0;

  for (word=strtok(words," "); word != NULL; word=strtok(NULL," ")) {
    length = (Int32)strlen(line);
    if (length > 0 && length + 1 + (Int32)strlen(word) > width) {
      add_line(commands,first,line,start);
      strcpy(line,"");
      length = 0;
      first = 0;
      width = LABEL_WIDTH - INDENT;
      }
    if (length == 0) start = (Int32)(word - words);
      else strcat(line," ");
    strncat(line,word,width - (Int32)strlen(line));
    }

  add_line(commands,first,line,start);
  }

/* Returns the command added, or NULL when the menu is full */
static COMMAND* add_command(COMMANDS* commands,char* label,char* keys,Int32 color) {
  COMMAND* command;

  if (commands->count == MAX_COMMANDS || commands->line_count == MAX_LINES) return NULL;

  command = &commands->list[commands->count++];
  sprintf(command->label,"%.*s",LABEL_LENGTH - 1,label);
  strcpy(command->keys,keys);
  command->action = DO_KEYS;
  command->color = color;
  command->enabled = 1;
  command->name_start = 0;
  command->name_color = 0;

  wrap_label(commands);
  return command;
  }

/* Shows the item name that follows verb in a command's label in its rarity colour, name_color, with the Improved
   Interface preference */
static void color_item_name(COMMAND* command,char* verb,Int32 name_color) {
  if (command == NULL || name_color == 0 || improved_interface != 'Y') return;

  command->name_start = (Int32)strlen(verb) + 1;
  command->name_color = name_color;
  }

/* Save and Load are greyed out for a character with permadeath, whose save is held while it plays */
static void add_action(COMMANDS* commands,char* label,Int32 action) {
  add_command(commands,label,"",COMMON_COLOR);
  commands->list[commands->count - 1].action = action;
  commands->list[commands->count - 1].enabled = !has_permadeath();
  }

/* "<verb> <name>", with the name the look command gives a square of this value. Returns the rarity colour of an item's
   name, or 0 for a square that holds no item. */
static Int32 named_label(char* verb,Int32 value,char* label) {
  char  name[80];
  Int32 color;
  Int32 i;

  color = 0;
  square_name(value,name,&color);
  sprintf(label,"%s %.60s",verb,name);
  for (i=0; label[i] != 0; i++)
    if (label[i] == '_') label[i] = ' ';

  return color;
  }

/* The map value of the square in direction d, or 0 off the map's edge */
static Int32 value_beside(Int32 d) {
  Int32 x,y;

  x = player.x + step_x[d];
  y = player.y + step_y[d];
  if (x < 1 || x > 100 || y < 1 || y > 100) return 0;

  return player.map[x][y];
  }

static Int32 count_beside(Int32 value) {
  Int32 count;
  Int32 d;

  count = 0;
  for (d=0; d<8; d++)
    if (value_beside(d) == value) count++;

  return count;
  }

/* A command aimed at the square in direction d: its key alone when the game picks that square itself (aimed, as
   item_dir() finds it), its key and the direction when the game asks which; none when the game would pick another */
static void add_aimed(COMMANDS* commands,char* label,char key,char aimed,Int32 d) {
  char keys[3];

  keys[0] = key;
  keys[1] = 0;
  keys[2] = 0;
  if (aimed == ' ') keys[1] = step_key[d];
    else if (aimed != step_key[d]) return;

  add_command(commands,label,keys,CONTEXT_COLOR);
  }

/* A step onto the square in direction d, then the key that acts there, if one is needed */
static COMMAND* add_step(COMMANDS* commands,char* label,Int32 d,char then) {
  char keys[3];

  keys[0] = step_key[d];
  keys[1] = then;
  keys[2] = 0;
  return add_command(commands,label,keys,CONTEXT_COLOR);
  }

/* A creature beside you: talk to a friendly one, which t cannot reach beside a merchant, and attack the others by
   stepping into them */
static void add_creature(COMMANDS* commands,Int32 d,Int32 can_talk) {
  NPC_REC* creature;
  char     label[80];
  char     keys[2];

  creature = get_monster(player.x + step_x[d],player.y + step_y[d]);
  if (creature == NULL) return;

  if ((creature->special & MS_FRIENDLY) != 0) {
    if (!can_talk) return;
    sprintf(label,"Talk to %s",creature->name);
    add_aimed(commands,label,'t',item_dir(-1),d);
    return;
    }

  sprintf(label,"Attack %s",creature->name);
  keys[0] = step_key[d];
  keys[1] = 0;
  add_command(commands,label,keys,CONTEXT_COLOR);
  }

/* Whatever stands or lies on the square in direction d */
static void list_beside(COMMANDS* commands,Int32 d,Int32 can_talk) {
  Int32 value;
  Int32 color;
  char  label[80];

  value = value_beside(d);

  if (value < 0) add_creature(commands,d,can_talk);
  if (value == 22 || value == 33) add_aimed(commands,"Open the door",'o',door_to_open(),d);
  if (value == 23) add_aimed(commands,"Close the door",'c',item_dir(23),d);
  if (value >= 1000 && value < 2000) {
    color = named_label("Pack",value,label);
    color_item_name(add_step(commands,label,d,'G'),"Pack",color);
    }
  if (value >= 2000 && value < 3000) {
    named_label("Read",value,label);
    add_step(commands,label,d,'g');
    }

  /* Stepping onto a chest or torch picks it up when the auto-pickup preference does */
  if (value == 79) add_step(commands,"Open the chest",d,(auto_pickup_gold == 'Y') ? 0 : 'g');
  if (value == 84) add_step(commands,"Pick up the torch",d,(auto_pickup_torch == 'Y') ? 0 : 'g');

  if (value == 24) add_step(commands,"Go down the stairs",d,'>');
  if (value == 25) add_step(commands,"Go up the stairs",d,'<');
  if (value >= 901 && value <= 910) {
    named_label("Activate",value,label);
    add_step(commands,label,d,'A');
    }
  }

static void list_underfoot(COMMANDS* commands) {
  Int32 here;
  Int32 color;
  char  label[80];

  here = player.map[player.x][player.y];
  if (here >= 1000 && here < 2000) {
    color = named_label("Pack",here,label);
    color_item_name(add_command(commands,label,"G",CONTEXT_COLOR),"Pack",color);
    }
  if (here >= 2000 && here < 3000) {
    named_label("Read",here,label);
    add_command(commands,label,"g",CONTEXT_COLOR);
    }
  if (here == 79) add_command(commands,"Open the chest","g",CONTEXT_COLOR);
  if (here == 84) add_command(commands,"Pick up the torch","g",CONTEXT_COLOR);
  if (here == 24) add_command(commands,"Go down the stairs",">",CONTEXT_COLOR);
  if (here == 25) add_command(commands,"Go up the stairs","<",CONTEXT_COLOR);
  if (here >= 901 && here <= 910) {
    named_label("Activate",here,label);
    add_command(commands,label,"A",CONTEXT_COLOR);
    }
  }

/* In a town or dungeon: what lies under you, the merchant or house beside you, then everything on the squares around
   you, clockwise from north */
static void list_surroundings(COMMANDS* commands) {
  Int32 shop;
  Int32 d;

  list_underfoot(commands);

  shop = merchant_beside();
  if (shop >= 0) add_command(commands,shops[shop].name,"b",CONTEXT_COLOR);
  if (count_beside(S_SOLD) > 0) add_command(commands,"Your house","b",CONTEXT_COLOR);
  if (count_beside(S_SALE) > 0) add_command(commands,"House for sale","b",CONTEXT_COLOR);
  if (count_beside(536) > 0) add_command(commands,"Gacha machine","b",CONTEXT_COLOR);

  for (d=0; d<8; d++) list_beside(commands,d,shop < 0);
  }

static void list_town_and_dungeon(COMMANDS* commands) {
  list_surroundings(commands);

  add_command(commands,"Inventory","i",COMMON_COLOR);
  add_command(commands,"Use an item","u",COMMON_COLOR);
  if (player.mmana > 0) add_command(commands,"Cast a spell","m",COMMON_COLOR);
  add_command(commands,"Eat","e",COMMON_COLOR);
  if (player.in_city != 'Y') {
    add_command(commands,"Search","s",COMMON_COLOR);
    add_command(commands,"Light a torch","l",COMMON_COLOR);
    }
  add_command(commands,"Nap","n",COMMON_COLOR);
  add_command(commands,"Party","p",COMMON_COLOR);
  add_command(commands,"Quests","q",COMMON_COLOR);
  add_command(commands,"Message log","L",COMMON_COLOR);
  add_command(commands,"Character","R",COMMON_COLOR);
  add_command(commands,"Crafting","C",COMMON_COLOR);
  add_command(commands,"Look around","v",COMMON_COLOR);
  add_command(commands,"Location","~",COMMON_COLOR);
  add_command(commands,"Icons","H",COMMON_COLOR);
  add_command(commands,"Help","?",COMMON_COLOR);
  add_command(commands,"Options","O",COMMON_COLOR);
  }

/* What > enters on an overworld square of this value, or NULL */
static char* entrance(Int32 value) {
  switch (value) {
    case  5: return "Enter the town";
    case 17: return "Enter the village";
    case 11: return "Enter the dungeon";
    case 31: return "Enter the Gateway";
    }
  return NULL;
  }

/* On foot, the entrances and your ship on the squares around you */
static void list_land_beside(COMMANDS* commands,Int32 d) {
  Int32 value;

  value = value_beside(d);

  if (entrance(value) != NULL) add_step(commands,entrance(value),d,'>');

  /* With Ship Improvements walking onto your ship boards it */
  if (value == 12) add_step(commands,"Board the ship",d,(change_ship_improvements == 'Y') ? 0 : 'E');
  }

static void list_overworld(COMMANDS* commands) {
  Int32 here;
  Int32 d;

  here = player.map[player.x][player.y];
  if (entrance(here) != NULL) add_command(commands,entrance(here),">",CONTEXT_COLOR);
  if (player.inship != ' ') add_command(commands,"Leave the ship","E",CONTEXT_COLOR);
    else if (here == 12) add_command(commands,"Board the ship","E",CONTEXT_COLOR);
  if (player.inship == ' ')
    for (d=0; d<8; d++) list_land_beside(commands,d);
  if (has_intrinsic(A_TARD)) add_command(commands,"Tardis log","T",CONTEXT_COLOR);

  add_command(commands,"Inventory","i",COMMON_COLOR);
  add_command(commands,"Use an item","u",COMMON_COLOR);
  add_command(commands,"Eat","e",COMMON_COLOR);
  add_command(commands,"Hunt","h",COMMON_COLOR);
  add_command(commands,"Look around","v",COMMON_COLOR);
  add_command(commands,"Character","R",COMMON_COLOR);
  add_command(commands,"Location","~",COMMON_COLOR);
  add_command(commands,"Icons","H",COMMON_COLOR);
  add_command(commands,"Help","?",COMMON_COLOR);
  add_command(commands,"Options","O",COMMON_COLOR);
  }

static void list_commands(COMMANDS* commands) {
  commands->count = 0;
  commands->line_count = 0;

  if (in_upland == 'Y') list_overworld(commands);
    else list_town_and_dungeon(commands);

  add_action(commands,"Save",DO_SAVE);
  add_action(commands,"Load",DO_LOAD);
  }

/* The log's box as a list: its first line holds the heading, its last the "more v" marker */
static SCROLL_LIST panel_rows(Int32 count) {
  SCROLL_LIST rows = { .count = count, .shown = NUM_MSG - 2, .line = 1, .more_column = MORE_COLUMN,
                       .x = screen_width - PANEL_LEFT, .y = PANEL_TOP, .row_height = MESSAGE_HEIGHT };

  return rows;
  }

static void open_panel(MENU* menu,Int32 count,void (*draw_row)(MENU* menu,Int32 row,Int32 line),void* screen) {
  menu->rows = panel_rows(count);
  menu->row = 0;
  menu->column = 1;
  menu->value_column = KEY_COLUMN;
  menu->color = COMMON_COLOR;
  menu->keys = NULL;
  menu->keys_line = 0;
  menu->draw_row = draw_row;
  menu->screen = screen;
  }

/* Clears the log's box and shows the menu there, under its heading */
static void show_panel(MENU* menu,char* heading) {
  clear_messages();
  list_text(&menu->rows,1,1,HEADING_COLOR,heading);
  draw_menu(menu);
  }

/* The column in a line's text where the command's item name starts, or -1 when the line shows none of it */
static Int32 name_column(COMMAND* command,COMMAND_LINE* text) {
  Int32 indent;
  Int32 at;

  if (command->name_color == 0) return -1;

  indent = text->first ? 0 : INDENT;
  at = command->name_start - text->offset;
  if (at < 0) at = 0;
  if (indent + at >= (Int32)strlen(text->text)) return -1;

  return indent + at;
  }

/* A line of a command, with the keys it types on its first, and an item's name in its rarity colour */
static void draw_command(MENU* menu,Int32 row,Int32 line) {
  COMMANDS*     commands;
  COMMAND_LINE* text;
  COMMAND*      command;
  char          keys[3];
  Int32         at;

  commands = menu->screen;
  text = &commands->lines[row];
  command = &commands->list[text->command];
  sprintf(keys,"%-2s",text->first ? command->keys : "");

  draw_menu_text_in(menu,line,text->text,keys,command->enabled ? command->color : 8);

  at = name_column(command,text);
  if (at >= 0)
    list_text(&menu->rows,menu->value_column - menu_label_width(menu) + at,line,command->name_color,text->text + at);
  }

/* Keeps the cursor on the first line of a command, with all its lines in sight: a step down onto a command's later
   line goes on to the next command, and any other move back to the first line of the command it reached */
static void keep_on_command(MENU* menu,COMMANDS* commands,char key) {
  Int32 row;
  Int32 last;
  Int32 scrolled;

  row = menu->row;
  if (key == '2')
    while (row < commands->line_count && !commands->lines[row].first) row++;
  if (row == commands->line_count) row = 0;
  while (!commands->lines[row].first) row--;

  last = row;
  while (last + 1 < commands->line_count && !commands->lines[last + 1].first) last++;

  menu->row = row;
  scrolled = list_scroll_to(&menu->rows,last);
  scrolled |= list_scroll_to(&menu->rows,row);
  if (scrolled) draw_menu(menu);
  }

/* A saved game and its character's level */
static void draw_game(MENU* menu,Int32 row,Int32 line) {
  SAVED_GAME* game;
  char        level[8];

  game = &((SAVE_LIST*)menu->screen)->games[row];
  if (game->readable) sprintf(level,"%-2d",game->summary.level);
    else strcpy(level,"  ");

  draw_menu_text_in(menu,line,game->name,level,game->readable ? COMMON_COLOR : 8);
  }

static Int32 save_exists(char* name) {
  char  filename[sizeof(loadname) + 4];
  char* extensions[2] = { ".sav", ".bak" };
  FILE* file;
  Int32 i;

  for (i=0; i<2; i++) {
    sprintf(filename,"%s%s",name,extensions[i]);
    file = fopen(filename,"r");
    if (file != NULL) {
      fclose(file);
      return 1;
      }
    }

  return 0;
  }

static Int32 is_name_key(char key) {
  return (isalnum((unsigned char)key) || key == ' ' || key == '-' || key == '_');
  }

/* Yes or No to saving over the game already saved under name; No is where the cursor starts */
static Int32 confirm_overwrite(char* name) {
  MENU menu;
  char heading[LABEL_WIDTH + 8];

  sprintf(heading,"Overwrite %.*s?",LABEL_WIDTH - 10,name);
  open_panel(&menu,2,draw_menu_choice,overwrite_choices);
  menu.row = 1;
  show_panel(&menu,heading);

  return (menu_choice(&menu) == 0);
  }

static void show_name_prompt(SCROLL_LIST* panel) {
  clear_messages();
  list_text(panel,1,1,HEADING_COLOR,"Save as New");
  list_text(panel,1,2,COMMON_COLOR,"Name:");
  }

/* Reads the name for Save as New in the box; a name already saved under asks to overwrite it. Returns 0 when ESC
   backs out. */
static Int32 ask_save_name(char* name) {
  SCROLL_LIST panel;
  char        typed[NAME_LENGTH + 2];
  char        field[NAME_LENGTH + 2];
  char        problem[LABEL_WIDTH + 1];
  char        key;
  Int32       length;

  panel = panel_rows(0);
  show_name_prompt(&panel);

  strcpy(name,"");
  strcpy(problem,"");
  key = 0;
  while (key != 13) {
    sprintf(typed,"%s_",name);
    sprintf(field,"%-*s",NAME_LENGTH + 1,typed);
    list_text(&panel,1,3,15,field);
    sprintf(field,"%-*s",LABEL_WIDTH,problem);
    list_text(&panel,1,5,12,field);

    key = inkey();
    length = (Int32)strlen(name);
    strcpy(problem,"");

    if (key == 27) return 0;
    if ((key == 8 || key == 127) && length > 0) name[length - 1] = 0;
    if (is_name_key(key) && length < NAME_LENGTH) {
      name[length] = key;
      name[length + 1] = 0;
      }

    if (key == 13 && length < 2) {
      strcpy(problem,"Too short");
      key = 0;
      }
    if (key == 13 && save_exists(name) && !confirm_overwrite(name)) {
      show_name_prompt(&panel);
      key = 0;
      }
    }

  return 1;
  }

static void save_here(char* name) {
  char filename[sizeof(loadname)];

  /* On the overworld the save keeps your square as the land square, as S does */
  if (in_upland == 'Y') {
    player.land_x = player.x;
    player.land_y = player.y;
    }

  strcpy(filename,name);
  save_game(filename);
  }

/* Overwrite, Save as New or Cancel, under the heading. Returns 1 once the game is saved. */
static Int32 save_choice(char* heading) {
  MENU  menu;
  char  name[sizeof(loadname)];
  Int32 row;

  open_panel(&menu,3,draw_menu_choice,save_choices);
  row = -2;
  while (row == -2) {
    show_panel(&menu,heading);
    row = menu_choice(&menu);
    if (row == 1 && !ask_save_name(name)) row = -2;
    }

  if (row == 0) save_here((strlen(loadname) > 0) ? loadname : player.name);
  if (row == 1) save_here(name);

  return (row == 0 || row == 1);
  }

/* Saves the game, then picks a saved game, whose name goes in loadname. Returns 1 when one was picked. */
static Int32 load_choice(void) {
  SAVE_LIST games;
  MENU      menu;
  Int32     row;

  if (!save_choice("Save, then Load")) return 0;

  open_save_list(&games);
  open_panel(&menu,games.count,draw_game,&games);
  row = -2;
  while (row == -2) {
    show_panel(&menu,"Load which game?");
    row = menu_choice(&menu);
    if (row >= 0 && !games.games[row].readable) row = -2;
    }

  if (row >= 0) strcpy(loadname,games.games[row].name);
  close_save_list(&games);

  return (row >= 0);
  }

/* Opens the menu, and TAB or ESC closes it. Returns the first key of the command chosen, its others queued to follow;
   0 when none was; TAB_LOAD_GAME when a game to load was picked. */
Int32 open_tab_menu(void) {
  COMMANDS commands;
  COMMAND* command;
  MENU     menu;
  Int32    result;
  Int32    saved;
  Int32    choosing;
  char     key;
  char     text[sizeof(loadname) + 16];

  list_commands(&commands);
  open_panel(&menu,commands.line_count,draw_command,&commands);
  show_panel(&menu,"Menu");

  result = 0;
  saved = 0;
  choosing = 1;
  while (choosing) {
    key = menu_key(&menu);
    keep_on_command(&menu,&commands,key);
    if (key == 9 || key == 27) choosing = 0;

    command = &commands.list[commands.lines[menu.row].command];
    if (key != 13 || !command->enabled) continue;

    if (command->action == DO_SAVE) {
      saved = save_choice("Save");
      choosing = !saved;
      }
    else if (command->action == DO_LOAD) {
      if (load_choice()) {
        result = TAB_LOAD_GAME;
        choosing = 0;
        }
      }
    else {
      strcpy(currentMacro,command->keys + 1);
      result = (unsigned char)command->keys[0];
      choosing = 0;
      }

    if (choosing) show_panel(&menu,"Menu");
    }

  show_messages();
  if (saved) {
    sprintf(text,"Saved as %s",loadname);
    msg(text,15);
    }

  return result;
  }

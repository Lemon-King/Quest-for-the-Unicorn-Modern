/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The Tab Menu, for the Improved Interface preference: in the box that shows the log, the commands for where you
   stand, then the common ones, then Save, Load and Save and Exit, which an arena fight leaves out. A command types its
   keys into the game, as if pressed. */

#include "header.h"

#include <ctype.h>

#define MAX_COMMANDS    64     /* in the menu itself */
#define LINES_EACH       4     /* lines a label may wrap onto */
#define LABEL_WIDTH     20
#define LABEL_LENGTH    72
#define INDENT           1     /* a label's later lines, under its first */
#define NAME_LENGTH     20     /* of a save named with Save as New */
#define PANEL_LEFT     215     /* from the right edge, where the log's lines start */
#define PANEL_TOP      100
#define MORE_COLUMN     19
#define BAR_COLUMN      26     /* the scroll bar's */
#define KEY_COLUMN      24     /* a space after a command's label, the keys it types */
#define MANA_COLUMN     23     /* a space after a spell's name, its mana */
#define STATUS_WIDTH    25     /* the columns a status's description wraps at */
#define STATUS_KEYS     "(4/6) Move  (ESC) Back"

#define CONTEXT_COLOR   10
#define COMMON_COLOR    14
#define HEADING_COLOR   11

#define DO_KEYS          0
#define DO_SAVE          1
#define DO_LOAD          2
#define DO_SPELLS        3
#define DO_STATUS        4

/* A command: what it says, the keys it types (the first returned, the rest queued), or Save, Load, the Easy Spells
   list or Status Effects. An item's name in its label, from name_start on, shows in its rarity colour, name_color; 0
   for none. */
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

/* Room for some commands, and the lines their labels wrap onto, width columns wide */
typedef struct {
  COMMAND*      list;
  Int32         count;
  Int32         room;
  COMMAND_LINE* lines;
  Int32         line_count;
  Int32         width;
  } COMMANDS;

static char* save_choices[3] = { "Overwrite", "Save as New", "Cancel" };
static char* overwrite_choices[2] = { "Yes", "No" };

static void open_commands(COMMANDS* commands,Int32 room,Int32 width) {
  commands->list = malloc(room * sizeof(COMMAND));
  commands->lines = malloc(room * LINES_EACH * sizeof(COMMAND_LINE));
  commands->room = (commands->list != NULL && commands->lines != NULL) ? room : 0;
  commands->count = 0;
  commands->line_count = 0;
  commands->width = width;
  }

static void close_commands(COMMANDS* commands) {
  free(commands->list);
  free(commands->lines);
  }

static void add_line(COMMANDS* commands,Int32 first,char* text,Int32 offset) {
  COMMAND_LINE* line;

  if (commands->line_count == commands->room * LINES_EACH) return;

  line = &commands->lines[commands->line_count++];
  line->command = commands->count - 1;
  line->first = first;
  line->offset = offset;
  sprintf(line->text,"%*s%s",first ? 0 : INDENT,"",text);
  }

/* The words of text from *at that fit on a line width columns wide, into line; a word too long for a line is cut.
   Sets *start to where the line begins in text and moves *at past it. Returns 0 when no words are left. */
static Int32 next_line(char* text,Int32* at,Int32 width,char* line,Int32* start) {
  Int32 length;
  Int32 size;
  Int32 kept;

  while (text[*at] == ' ') (*at)++;
  if (text[*at] == 0) return 0;

  *start = *at;
  length = 0;
  while (text[*at] != 0) {
    for (size=0; text[*at + size] != 0 && text[*at + size] != ' '; size++);
    if (length > 0 && length + 1 + size > width) break;

    if (length > 0) line[length++] = ' ';
    kept = (size < width - length) ? size : width - length;
    memcpy(line + length,text + *at,kept);
    length += kept;

    *at += size;
    while (text[*at] == ' ') (*at)++;
    }
  line[length] = 0;

  return 1;
  }

/* Breaks the newest command's label into lines at its spaces, the later lines indented */
static void wrap_label(COMMANDS* commands) {
  char* label;
  char  line[LABEL_WIDTH + 1];
  Int32 at;
  Int32 start;
  Int32 first;

  label = commands->list[commands->count - 1].label;
  at = 0;
  first = 1;
  while (next_line(label,&at,first ? commands->width : commands->width - INDENT,line,&start)) {
    add_line(commands,first,line,start);
    first = 0;
    }

  if (first) add_line(commands,1,"",0);
  }

/* Returns the command added, or NULL when the menu is full */
static COMMAND* add_command(COMMANDS* commands,char* label,char* keys,Int32 color) {
  COMMAND* command;

  if (commands->count == commands->room || commands->line_count == commands->room * LINES_EACH) return NULL;

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

/* Shows the item name that follows verb in a command's label in its rarity colour, name_color */
static void color_item_name(COMMAND* command,char* verb,Int32 name_color) {
  if (command == NULL || name_color == 0) return;

  command->name_start = (Int32)strlen(verb) + 1;
  command->name_color = name_color;
  }

/* Save and Load are greyed out for a character with permadeath, whose save is held while it plays */
static void add_action(COMMANDS* commands,char* label,Int32 action) {
  add_command(commands,label,"",COMMON_COLOR);
  commands->list[commands->count - 1].action = action;
  commands->list[commands->count - 1].enabled = !has_permadeath();
  }

/* The next status you have after chosen, stepping by step (1 or -1) and wrapping round; -1 when you have none */
static Int32 next_status(Int32 chosen,Int32 step) {
  STATUS_NOTE note;
  Int32       at;
  Int32       i;

  for (i=1; i<=STATUS_COUNT; i++) {
    at = (chosen + step * i + STATUS_COUNT) % STATUS_COUNT;
    describe_status(at,&note);
    if (note.active) return at;
    }

  return -1;
  }

static Int32 first_status(void) {
  return next_status(STATUS_COUNT - 1,1);
  }

/* Status Effects, greyed out while you have none */
static void add_status_effects(COMMANDS* commands) {
  COMMAND* command;

  command = add_command(commands,"Status Effects","",COMMON_COLOR);
  if (command == NULL) return;

  command->action = DO_STATUS;
  command->enabled = (first_status() >= 0);
  }

/* Spells types m for the spell prompt, or with Easy Spells opens the list of your spells */
static void add_spells(COMMANDS* commands) {
  COMMAND* command;

  command = add_command(commands,"Spells","m",COMMON_COLOR);
  if (command != NULL && easy_spells == 'Y') command->action = DO_SPELLS;
  }

/* "<verb> <name>", with any underscores in the name as spaces */
static void verb_label(char* verb,char* name,char* label) {
  Int32 i;

  sprintf(label,"%s %.60s",verb,name);
  for (i=0; label[i] != 0; i++)
    if (label[i] == '_') label[i] = ' ';
  }

/* "<verb> <name>", with the name the look command gives a square of this value. Returns the colour it gives an item's
   name, or 0 for a square that holds no item. */
static Int32 named_label(char* verb,Int32 value,char* label) {
  char  name[80];
  Int32 color;

  color = 0;
  square_name(value,name,&color);
  verb_label(verb,name,label);

  return color;
  }

/* "<verb> <item>", with the item's name in its rarity colour */
static void add_item_command(COMMANDS* commands,char* verb,OBJECT_REC* item,char* keys) {
  char label[LABEL_LENGTH];

  verb_label(verb,get_name(item),label);
  color_item_name(add_command(commands,label,keys,COMMON_COLOR),verb,item_color(item));
  }

/* The square in direction d, as x,y; returns 0 off the map's edge */
static Int32 square_beside(Int32 d,Int32* x,Int32* y) {
  *x = player.x + step_x(d);
  *y = player.y + step_y(d);

  return (*x >= 1 && *x <= 100 && *y >= 1 && *y <= 100);
  }

/* The map value of the square in direction d, or 0 off the map's edge */
static Int32 value_beside(Int32 d) {
  Int32 x,y;

  if (!square_beside(d,&x,&y)) return 0;

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
  if (aimed == ' ') keys[1] = step_key(d);
    else if (aimed != step_key(d)) return;

  add_command(commands,label,keys,CONTEXT_COLOR);
  }

/* A step onto the square in direction d, then the key that acts there, if one is needed */
static COMMAND* add_step(COMMANDS* commands,char* label,Int32 d,char then) {
  char keys[3];

  keys[0] = step_key(d);
  keys[1] = then;
  keys[2] = 0;
  return add_command(commands,label,keys,CONTEXT_COLOR);
  }

/* The key Pick Up types: g, which takes the item through the inventory, to hold an item when the pack is full and,
   without the Unified Pickup change, for what only its pickup there counts (a dungeon's quest treasure completes the
   quest, a trading card joins the collection); G packs anything else */
static char pick_up_key(OBJECT_REC* item) {
  if (pack_count() == PACK_SLOTS) return 'g';
  if (change_unified_pickup == 'Y') return 'G';
  if (item->otype == O_CARD) return 'g';

  /* The Gateway has no row in links[] for is_quest_treasure() to read */
  if (player.dungeon_number != GATEWAY_DUNGEON && is_quest_treasure(item)) return 'g';

  return 'G';
  }

/* "Pick Up <item>" for the floor item of this map value, a step away in direction d, or underfoot when d is -1 */
static void add_pick_up(COMMANDS* commands,Int32 value,Int32 d) {
  COMMAND* command;
  char     label[80];
  char     keys[2];
  Int32    color;

  keys[0] = pick_up_key(&(player.map_objects[value - 1000]));
  keys[1] = 0;
  color = named_label("Pick Up",value,label);

  if (d >= 0) command = add_step(commands,label,d,keys[0]);
    else command = add_command(commands,label,keys,CONTEXT_COLOR);
  color_item_name(command,"Pick Up",color);
  }

/* A pacified creature, or a friendly one not yet asked, which j may ask to join the party */
static Int32 may_join(NPC_REC* creature) {
  if ((creature->special2 & M2_PACIFIED) != 0) return 1;

  return ((creature->special & MS_FRIENDLY) != 0 && (creature->special & MS_JOIN) == 0);
  }

/* Attack the creature in direction d by stepping into it */
static void add_attack(COMMANDS* commands,NPC_REC* creature,Int32 d) {
  char label[80];

  sprintf(label,"Attack %s",creature->name);
  add_step(commands,label,d,0);
  }

/* A creature beside you: talk to a friendly one, which t cannot reach beside a merchant, attack the others by stepping
   into them, and ask one that may join */
static void add_creature(COMMANDS* commands,Int32 d,Int32 can_talk) {
  NPC_REC* creature;
  char     label[80];

  creature = get_monster(player.x + step_x(d),player.y + step_y(d));
  if (creature == NULL) return;

  if ((creature->special & MS_FRIENDLY) != 0) {
    sprintf(label,"Talk to %s",creature->name);
    if (can_talk) add_aimed(commands,label,'t',item_dir(-1),d);
    }
  else add_attack(commands,creature,d);

  if (may_join(creature)) {
    sprintf(label,"Ask %s to join",creature->name);
    add_aimed(commands,label,'j',item_dir(-1),d);
    }
  }

/* Whatever stands or lies on the square in direction d */
static void list_beside(COMMANDS* commands,Int32 d,Int32 can_talk) {
  Int32 value;
  char  label[80];

  value = value_beside(d);

  if (value < 0) add_creature(commands,d,can_talk);
  if (value == 22 || value == 33) add_aimed(commands,"Open the door",'o',door_to_open(),d);
  if (value == 23) add_aimed(commands,"Close the door",'c',item_dir(23),d);

  /* J always asks which way, and jams only a closed door north, south, east or west */
  if (value == 22 && d % 2 == 0) add_aimed(commands,"Jam the door",'J',' ',d);

  if (value >= 1000 && value < 2000) add_pick_up(commands,value,d);
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
  char  label[80];

  here = player.map[player.x][player.y];
  if (here >= 1000 && here < 2000) add_pick_up(commands,here,-1);
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

/* A bow, wand or projected energy weapon in the main hand, which f fires */
static Int32 can_fire(void) {
  Int32 type;

  type = player.invent[I_RHND].otype;

  return (type == O_BOW || type == O_WAND || type == O_PROJ);
  }

/* A dagger or throwing weapon, which - and = throw from the left and right hands */
static Int32 is_thrown(OBJECT_REC* item) {
  return (item->otype == O_DAGR || item->otype == O_THRN);
  }

static Int32 holds_spell_scroll(void) {
  Int32 i;

  for (i=I_RHND; i<=I_HELM; i++)
    if (player.invent[i].otype == O_SCRL && player.invent[i].effect == E_SPEL) return 1;

  return 0;
  }

/* The commands your gear, spells and wounds allow in an arena fight */
static void list_combat_abilities(COMMANDS* commands) {
  if (knows_a_spell()) add_spells(commands);
  if (can_fire()) add_item_command(commands,"Fire",&(player.invent[I_RHND]),"f");
  if (is_thrown(&(player.invent[I_LHND]))) add_item_command(commands,"Throw",&(player.invent[I_LHND]),"-");
  if (is_thrown(&(player.invent[I_RHND]))) add_item_command(commands,"Throw",&(player.invent[I_RHND]),"=");
  if (player.chp < player.mhp) add_command(commands,"Heal","h",COMMON_COLOR);
  }

/* The commands your gear, spells and wounds allow */
static void list_abilities(COMMANDS* commands) {
  list_combat_abilities(commands);
  if (holds_spell_scroll()) add_command(commands,"Transcribe a spell","x",COMMON_COLOR);
  if (word_counter == 0 && player.gateway != '*') add_command(commands,"Say a Word of Power","w",COMMON_COLOR);
  }

static void list_town_and_dungeon(COMMANDS* commands) {
  list_surroundings(commands);

  add_command(commands,"Inventory","i",COMMON_COLOR);
  add_command(commands,"Use an item","u",COMMON_COLOR);
  list_abilities(commands);
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
  add_status_effects(commands);
  add_command(commands,"Crafting","C",COMMON_COLOR);
  add_command(commands,"Look around","v",COMMON_COLOR);
  add_command(commands,"Location","~",COMMON_COLOR);
  add_command(commands,"Icons","H",COMMON_COLOR);
  add_command(commands,"Help","?",COMMON_COLOR);
  add_command(commands,"Options","O",COMMON_COLOR);
  }

/* What > enters on the overworld square x,y, as label, naming the town or dungeon; returns 0 for none */
static Int32 entrance(Int32 x,Int32 y,char* label) {
  char* place;

  place = entrance_name(x,y);
  if (place != NULL) {
    sprintf(label,"Enter %s",place);
    return 1;
    }

  switch (player.map[x][y]) {
    case  5: strcpy(label,"Enter the town"); return 1;
    case 17: strcpy(label,"Enter the village"); return 1;
    case 11: strcpy(label,"Enter the dungeon"); return 1;
    case 31: strcpy(label,"Enter the Gateway"); return 1;
    }

  return 0;
  }

/* On foot, the entrances and your ship on the squares around you */
static void list_land_beside(COMMANDS* commands,Int32 d) {
  char  label[LABEL_LENGTH];
  Int32 x,y;

  if (!square_beside(d,&x,&y)) return;

  if (entrance(x,y,label)) add_step(commands,label,d,'>');

  /* With Ship Improvements walking onto your ship boards it */
  if (player.map[x][y] == 12) add_step(commands,"Board the ship",d,(change_ship_improvements == 'Y') ? 0 : 'E');
  }

static void list_overworld(COMMANDS* commands) {
  char  label[LABEL_LENGTH];
  Int32 here;
  Int32 d;

  here = player.map[player.x][player.y];
  if (entrance(player.x,player.y,label)) add_command(commands,label,">",CONTEXT_COLOR);
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
  add_status_effects(commands);
  add_command(commands,"Location","~",COMMON_COLOR);
  if (world_map != 'N') add_command(commands,"World map","M",COMMON_COLOR);
  add_command(commands,"Icons","H",COMMON_COLOR);
  add_command(commands,"Help","?",COMMON_COLOR);
  add_command(commands,"Options","O",COMMON_COLOR);
  }

/* A monster on the square in direction d to attack, and an item there to pick up, unless the step leaves the arena */
static void list_arena_beside(COMMANDS* commands,Int32 d) {
  NPC_REC* monster;
  Int32    value;
  Int32    x,y;

  if (!square_beside(d,&x,&y)) return;

  monster = get_monster(x,y);
  if (monster != NULL) add_attack(commands,monster,d);

  value = player.map[x][y];
  if (value >= 1000 && value < 2000 && !off_arena(x,y)) add_pick_up(commands,value,d);
  }

/* In an arena fight: the item under you, the monsters and items around you, clockwise from north, the way out once the
   fight is won, then the commands the arena takes */
static void list_arena(COMMANDS* commands) {
  Int32 here;
  Int32 d;

  here = player.map[player.x][player.y];
  if (here >= 1000 && here < 2000) add_pick_up(commands,here,-1);

  for (d=0; d<8; d++) list_arena_beside(commands,d);

  /* Without the Arena Exit change ENTER never leaves */
  if (change_arena_exit == 'Y' && monsters_dead()) add_command(commands,"Leave Arena","\r",CONTEXT_COLOR);

  add_command(commands,"Inventory","i",COMMON_COLOR);
  add_command(commands,"Use an item","u",COMMON_COLOR);
  list_combat_abilities(commands);
  add_command(commands,"Party","p",COMMON_COLOR);
  add_command(commands,"Character","R",COMMON_COLOR);
  add_status_effects(commands);
  add_command(commands,"Look around","v",COMMON_COLOR);
  add_command(commands,"Icons","H",COMMON_COLOR);
  add_command(commands,"Help","?",COMMON_COLOR);
  }

static void list_commands(COMMANDS* commands) {
  /* The arena takes no Save, Load or X: its field stands in for the map until the fight ends */
  if (in_arena == 'Y') {
    list_arena(commands);
    return;
    }

  if (in_upland == 'Y') list_overworld(commands);
    else list_town_and_dungeon(commands);

  add_action(commands,"Save",DO_SAVE);
  add_action(commands,"Load",DO_LOAD);
  add_command(commands,"Save and Exit","X",COMMON_COLOR);
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

/* A line of a command, with its keys on its first up to the scroll bar, and an item's name in its rarity colour */
static void draw_command(MENU* menu,Int32 row,Int32 line) {
  COMMANDS*     commands;
  COMMAND_LINE* text;
  COMMAND*      command;
  char*         shown;
  char          keys[8];
  Int32         at;

  commands = menu->screen;
  text = &commands->lines[row];
  command = &commands->list[text->command];

  /* ENTER, which has no glyph, is typed but not shown */
  shown = (text->first && isprint((unsigned char)command->keys[0])) ? command->keys : "";
  sprintf(keys,"%-*s",BAR_COLUMN - menu->value_column,shown);

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

static Int32 first_line(COMMANDS* commands,Int32 command) {
  Int32 row;

  for (row=0; row<commands->line_count; row++)
    if (commands->lines[row].command == command) return row;

  return 0;
  }

/* Moves the cursor from command to command until ENTER picks one that is enabled. Returns it, or -1 when TAB or ESC
   backs out. */
static Int32 choose_command(MENU* menu,COMMANDS* commands) {
  Int32 picked;
  Int32 command;
  char  key;

  if (commands->line_count == 0) return -1;

  picked = -2;
  while (picked == -2) {
    key = menu_key(menu);
    keep_on_command(menu,commands,key);
    command = commands->lines[menu->row].command;

    if (key == 9 || key == 27) picked = -1;
    if (key == 13 && commands->list[command].enabled) picked = command;
    }

  return picked;
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

static Int32 can_cast(Int32 spell) {
  return (player.cmana >= mana_cost(spells[spell].mana));
  }

/* Of the spells known, the one cast last, which F1 keeps; the first when it is not among them */
static Int32 last_cast(SPELLBOOK known,Int32 count) {
  Int32 i;

  for (i=0; i<count; i++)
    if (strcmp(spells[known[i]].name,player.fspell[1]) == 0) return i;

  return 0;
  }

/* Easy Spells: picks one of the spells you know, each with its mana, though not one you lack the mana for. The cursor
   starts on the spell cast last. Returns the spell, or 0 for none. */
static Int32 spell_choice(void) {
  SPELLBOOK known;
  COMMANDS  choices;
  COMMAND*  choice;
  MENU      menu;
  Int32     count;
  Int32     picked;
  Int32     i;
  char      mana[8];

  count = known_spells(known);
  open_commands(&choices,count,LABEL_WIDTH - 1);
  for (i=0; i<count; i++) {
    sprintf(mana,"%3d",mana_cost(spells[known[i]].mana));
    choice = add_command(&choices,spells[known[i]].name,mana,COMMON_COLOR);
    if (choice != NULL) choice->enabled = can_cast(known[i]);
    }

  picked = -1;
  if (choices.line_count > 0) {
    open_panel(&menu,choices.line_count,draw_command,&choices);
    menu.value_column = MANA_COLUMN;
    menu.row = first_line(&choices,last_cast(known,count));
    keep_on_command(&menu,&choices,0);
    show_panel(&menu,"Cast which spell?");
    picked = choose_command(&menu,&choices);
    }

  close_commands(&choices);

  return (picked >= 0) ? known[picked] : 0;
  }

/* text on the box's lines from line on, wrapped at its spaces. Returns the line after it. */
static Int32 write_wrapped(SCROLL_LIST* panel,Int32 line,Int32 color,char* text) {
  char  words[STATUS_WIDTH + 1];
  Int32 at;
  Int32 start;

  at = 0;
  while (next_line(text,&at,STATUS_WIDTH,words,&start)) list_text(panel,1,line++,color,words);

  return line;
  }

/* A ^ under status chosen in the row above the box, and in the box its name, what it does and how long it lasts */
static void show_status(SCROLL_LIST* panel,Int32 chosen) {
  STATUS_NOTE note;
  Int32       line;

  describe_status(chosen,&note);
  clear_messages();

  /* The ^ goes under the status's own cell, which need not fall on the box's columns */
  setcolor(15);
  outtextxy(note.x,panel->y + 10,"^");

  list_text(panel,1,2,note.active ? note.color : 8,note.name);
  line = write_wrapped(panel,3,7,note.effect);
  write_wrapped(panel,line + 1,note.active ? 14 : 8,note.lasting);
  list_text(panel,1,NUM_MSG,3,STATUS_KEYS);
  }

/* Status Effects, chosen only while you have one: 4 and 6 move the ^ from status to status you have, and ESC goes
   back */
static void browse_statuses(void) {
  SCROLL_LIST panel;
  Int32       chosen;
  char        key;

  panel = panel_rows(0);
  chosen = first_status();
  if (chosen < 0) return;

  key = 0;
  while (key != 27) {
    show_status(&panel,chosen);
    key = inkey();
    if (key == '4') chosen = next_status(chosen,-1);
    if (key == '6') chosen = next_status(chosen,1);
    }
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
  Int32    picked;
  Int32    spell;
  char     text[sizeof(loadname) + 16];

  open_commands(&commands,MAX_COMMANDS,LABEL_WIDTH);
  list_commands(&commands);
  open_panel(&menu,commands.line_count,draw_command,&commands);
  show_panel(&menu,"Menu");

  result = 0;
  saved = 0;
  choosing = 1;
  while (choosing) {
    picked = choose_command(&menu,&commands);
    choosing = 0;

    if (picked >= 0) {
      command = &commands.list[picked];

      if (command->action == DO_SAVE) {
        saved = save_choice("Save");
        choosing = !saved;
        }
      else if (command->action == DO_LOAD) {
        if (load_choice()) result = TAB_LOAD_GAME;
          else choosing = 1;
        }
      else if (command->action == DO_SPELLS) {
        spell = spell_choice();
        if (spell != 0) {
          pick_spell(spell);
          result = (unsigned char)command->keys[0];
          }
        else choosing = 1;
        }
      else if (command->action == DO_STATUS) {
        browse_statuses();
        choosing = 1;
        }
      else {
        strcpy(currentMacro,command->keys + 1);
        result = (unsigned char)command->keys[0];
        }
      }

    if (choosing) show_panel(&menu,"Menu");
    }

  close_commands(&commands);
  show_messages();
  if (saved) {
    sprintf(text,"Saved as %s",loadname);
    msg(text,15);
    }

  return result;
  }

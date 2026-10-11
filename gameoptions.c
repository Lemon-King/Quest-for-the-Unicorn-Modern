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
#define NUM_CHANGES    46
#define FIRST_CHANGE   (NUM_RULES + NUM_PREFS)
#define NUM_OPTIONS    (FIRST_CHANGE + NUM_CHANGES)

#define SIDE_LEFT       0
#define SIDE_RIGHT      1

#define RULES_LINE      2
#define PREFS_LINE     11
#define CHANGES_LINE    2
#define HELP_LINE      23
#define LEFT_COLUMN     3
#define RIGHT_COLUMN   42
#define ARROW_INDENT    2
#define LABEL_INDENT    6
#define VALUE_INDENT   31
#define CHANGES_SHOWN  19

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

/* The first gameplay change the scrolling column shows */
static Int32 changes_top;

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
    .config = "MusicVolume", .apply = set_music_volume },
  { .label = "Eshter Spawns",
    .help  = "Eshter, the final boss, waits on the fifth level of the Gateway",
    .kind  = OPT_SWITCH, .flag = &change_eshter_spawns, .on = 'Y', .off = 'N', .key = "EshterSpawns" },
  { .label = "Dalek Spawns",
    .help  = "Daleks guard the Tardis on the bottom level of the Den of Horror",
    .kind  = OPT_SWITCH, .flag = &change_dalek_spawns, .on = 'Y', .off = 'N', .key = "DalekSpawns" },
  { .label = "Elemental Weapon Damage",
    .help  = "Equal physical and elemental weapon damage no longer deals 0",
    .kind  = OPT_SWITCH, .flag = &change_weapon_damage, .on = 'Y', .off = 'N', .key = "ElementalWeaponDamage" },
  { .label = "Scroll of Charging",
    .help  = "Scroll of Charging restores charges instead of freezing the game",
    .kind  = OPT_SWITCH, .flag = &change_scroll_charging, .on = 'Y', .off = 'N', .key = "ScrollOfCharging" },
  { .label = "No Negative Amounts",
    .help  = "Negative bank, house and dice amounts count as 0 instead of adding gold",
    .kind  = OPT_SWITCH, .flag = &change_negative_amounts, .on = 'Y', .off = 'N', .key = "NoNegativeAmounts" },
  { .label = "Teleport Spell",
    .help  = "Teleport moves you across a dungeon level instead of doing nothing",
    .kind  = OPT_SWITCH, .flag = &change_teleport_spell, .on = 'Y', .off = 'N', .key = "TeleportSpell" },
  { .label = "Blink Only Blinks",
    .help  = "Blink no longer also casts Protection",
    .kind  = OPT_SWITCH, .flag = &change_blink_only, .on = 'Y', .off = 'N', .key = "BlinkOnlyBlinks" },
  { .label = "Blink Landing",
    .help  = "Blink leaves you where it lands, never in a wall near the map's edge",
    .kind  = OPT_SWITCH, .flag = &change_blink_landing, .on = 'Y', .off = 'N', .key = "BlinkLanding" },
  { .label = "Sleep Traps",
    .help  = "Sleep traps put you to sleep instead of never springing",
    .kind  = OPT_SWITCH, .flag = &change_sleep_traps, .on = 'Y', .off = 'N', .key = "SleepTraps" },
  { .label = "Tardis Saves",
    .help  = "Saves keep Tardis destinations without losing lore or your position",
    .kind  = OPT_SWITCH, .flag = &change_tardis_saves, .on = 'Y', .off = 'N', .key = "TardisSaves" },
  { .label = "Tardis Descriptions",
    .help  = "Tardis descriptions stop at 31 characters instead of overrunning memory",
    .kind  = OPT_SWITCH, .flag = &change_tardis_descriptions, .on = 'Y', .off = 'N', .key = "TardisDescriptions" },
  { .label = "Item Deck",
    .help  = "The item deck never deals the broken \"EOL\" item",
    .kind  = OPT_SWITCH, .flag = &change_item_deck, .on = 'Y', .off = 'N', .key = "ItemDeck" },
  { .label = "Tripped Card",
    .help  = "The Deck of Many Things' Tripped card costs your own class's hit die",
    .kind  = OPT_SWITCH, .flag = &change_tripped_card, .on = 'Y', .off = 'N', .key = "TrippedCard" },
  { .label = "Lore Search",
    .help  = "Library searches show and learn the message that matched, not the next",
    .kind  = OPT_SWITCH, .flag = &change_lore_search, .on = 'Y', .off = 'N', .key = "LoreSearch" },
  { .label = "Library Text",
    .help  = "Town libraries show messages in plain text instead of enciphered",
    .kind  = OPT_SWITCH, .flag = &change_library_text, .on = 'Y', .off = 'N', .key = "LibraryText" },
  { .label = "Library Lines",
    .help  = "Town libraries write each line of a message below the last, not over it",
    .kind  = OPT_SWITCH, .flag = &change_library_lines, .on = 'Y', .off = 'N', .key = "LibraryLines" },
  { .label = "Arena Assassins",
    .help  = "Assassins keep their level in arena combat instead of restarting at 1",
    .kind  = OPT_SWITCH, .flag = &change_arena_assassins, .on = 'Y', .off = 'N', .key = "ArenaAssassins" },
  { .label = "Party Joining",
    .help  = "New party members get their party attack, defense and mana",
    .kind  = OPT_SWITCH, .flag = &change_party_joining, .on = 'Y', .off = 'N', .key = "PartyJoining" },
  { .label = "Spell Targets",
    .help  = "Blind, Sleep, Drain and reflected spells affect the creature they hit",
    .kind  = OPT_SWITCH, .flag = &change_spell_targets, .on = 'Y', .off = 'N', .key = "SpellTargets" },
  { .label = "Blast Spells",
    .help  = "Nuke, Nova and other blasts respect anti-magic and only anger friends",
    .kind  = OPT_SWITCH, .flag = &change_blast_spells, .on = 'Y', .off = 'N', .key = "BlastSpells" },
  { .label = "Spell Kills",
    .help  = "Spell kills drop loot, count as kills and share exp like other kills",
    .kind  = OPT_SWITCH, .flag = &change_spell_kills, .on = 'Y', .off = 'N', .key = "SpellKills" },
  { .label = "Thrown Weapons",
    .help  = "Thrown rocks and stars no longer crash the game when they land on an item",
    .kind  = OPT_SWITCH, .flag = &change_thrown_weapons, .on = 'Y', .off = 'N', .key = "ThrownWeapons" },
  { .label = "Poison Kills",
    .help  = "A creature killed by poison leaves the map where it died, not under you",
    .kind  = OPT_SWITCH, .flag = &change_poison_kills, .on = 'Y', .off = 'N', .key = "PoisonKills" },
  { .label = "Temple Uncurse",
    .help  = "Leaving a temple's uncurse without picking an item no longer crashes",
    .kind  = OPT_SWITCH, .flag = &change_temple_uncurse, .on = 'Y', .off = 'N', .key = "TempleUncurse" },
  { .label = "Ship Purchase",
    .help  = "A bought ship is yours at once, so a second one cannot be bought",
    .kind  = OPT_SWITCH, .flag = &change_ship_purchase, .on = 'Y', .off = 'N', .key = "ShipPurchase" },
  { .label = "Pack Weight",
    .help  = "The item in the last pack slot counts toward the weight you carry",
    .kind  = OPT_SWITCH, .flag = &change_pack_weight, .on = 'Y', .off = 'N', .key = "PackWeight" },
  { .label = "Possessions",
    .help  = "Items you used up or threw away no longer count as still yours",
    .kind  = OPT_SWITCH, .flag = &change_possessions, .on = 'Y', .off = 'N', .key = "Possessions" },
  { .label = "Manual of Bodily Health",
    .help  = "The Manual of Bodily Health no longer also uses the Crown of Might",
    .kind  = OPT_SWITCH, .flag = &change_bodily_health, .on = 'Y', .off = 'N', .key = "BodilyHealth" },
  { .label = "Compass",
    .help  = "The compass points the right way to and from the eastern regions",
    .kind  = OPT_SWITCH, .flag = &change_compass, .on = 'Y', .off = 'N', .key = "Compass" },
  { .label = "Gateway Exit",
    .help  = "Leaving the Gateway ends its rules, so later dungeons are normal again",
    .kind  = OPT_SWITCH, .flag = &change_gateway_exit, .on = 'Y', .off = 'N', .key = "GatewayExit" },
  { .label = "New Game Dungeon",
    .help  = "A new character's first visit to the Temple of the Forest builds it",
    .kind  = OPT_SWITCH, .flag = &change_new_game_dungeon, .on = 'Y', .off = 'N', .key = "NewGameDungeon" },
  { .label = "Words of Power",
    .help  = "Rising from a first level leaves it; sinking does nothing in a town",
    .kind  = OPT_SWITCH, .flag = &change_words_of_power, .on = 'Y', .off = 'N', .key = "WordsOfPower" },
  { .label = "Sun Stone",
    .help  = "Returning by Sun Stone no longer saves the town over the level above",
    .kind  = OPT_SWITCH, .flag = &change_sun_stone, .on = 'Y', .off = 'N', .key = "SunStone" },
  { .label = "Sun Stone Gateway",
    .help  = "A Sun Stone used in the Gateway brings you back to the Gateway",
    .kind  = OPT_SWITCH, .flag = &change_sun_stone_gateway, .on = 'Y', .off = 'N', .key = "SunStoneGateway" },
  { .label = "Upland Quit",
    .help  = "Q on the overworld asks before quitting, as it does in dungeons",
    .kind  = OPT_SWITCH, .flag = &change_upland_quit, .on = 'Y', .off = 'N', .key = "UplandQuit" },
  { .label = "Ranged Damage",
    .help  = "Arrows and thrown weapons roll one crit and count their damage once",
    .kind  = OPT_SWITCH, .flag = &change_ranged_damage, .on = 'Y', .off = 'N', .key = "RangedDamage" },
  { .label = "Summon Combat",
    .help  = "Monsters fight your summoned creature properly, and it can find you",
    .kind  = OPT_SWITCH, .flag = &change_summon_combat, .on = 'Y', .off = 'N', .key = "SummonCombat" },
  { .label = "House Storage",
    .help  = "All 500 house slots show, and leaving a full house keeps the item In Air",
    .kind  = OPT_SWITCH, .flag = &change_house_storage, .on = 'Y', .off = 'N', .key = "HouseStorage" },
  { .label = "House Buying",
    .help  = "Owning a house no longer stops you buying the others",
    .kind  = OPT_SWITCH, .flag = &change_house_buying, .on = 'Y', .off = 'N', .key = "HouseBuying" },
  { .label = "Shop Boots and Wands",
    .help  = "Armor shops sell boots, and magic shops sell wands",
    .kind  = OPT_SWITCH, .flag = &change_shop_boots_wands, .on = 'Y', .off = 'N', .key = "ShopBootsAndWands" },
  { .label = "Arena Exit",
    .help  = "ENTER or ESC leaves the arena once every monster is dead",
    .kind  = OPT_SWITCH, .flag = &change_arena_exit, .on = 'Y', .off = 'N', .key = "ArenaExit" },
  { .label = "Arena Decorations",
    .help  = "Trees and rocks in the arena never land on a monster or party member",
    .kind  = OPT_SWITCH, .flag = &change_arena_decorations, .on = 'Y', .off = 'N', .key = "ArenaDecorations" },
  { .label = "Arena Monk Fear",
    .help  = "Monks cannot be frightened in the arena from level 11, as in dungeons",
    .kind  = OPT_SWITCH, .flag = &change_arena_monk_fear, .on = 'Y', .off = 'N', .key = "ArenaMonkFear" },
  { .label = "Mind Shield",
    .help  = "Potions and scrolls of Mind Shield wear off like the other resistances",
    .kind  = OPT_SWITCH, .flag = &change_mind_shield, .on = 'Y', .off = 'N', .key = "MindShield" },
  { .label = "Meat and Potatoes",
    .help  = "Meat and Potatoes counts meat eaten, and meat counts once as a meal",
    .kind  = OPT_SWITCH, .flag = &change_meat_potatoes, .on = 'Y', .off = 'N', .key = "MeatAndPotatoes" },
  { .label = "World Seed",
    .help  = "New worlds pick from 2 billion dungeon layouts, not 32,767",
    .kind  = OPT_SWITCH, .flag = &change_world_seed, .on = 'Y', .off = 'N', .key = "WorldSeed" }
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

/* Rules and preferences are the left side of the screen, gameplay changes the right */
static Int32 option_side(Int32 i) {
  return is_gameplay_change(i) ? SIDE_RIGHT : SIDE_LEFT;
  }

static Int32 side_first(Int32 side) {
  return (side == SIDE_RIGHT) ? FIRST_CHANGE : 0;
  }

static Int32 side_end(Int32 side) {
  return (side == SIDE_RIGHT) ? NUM_OPTIONS : FIRST_CHANGE;
  }

static Int32 is_new_game_only(Int32 i) {
  return (i < NUM_RULES || is_gameplay_change(i));
  }

static Int32 is_editable(Int32 i, char rules_editable) {
  if (is_held_by_mode(&option_rows[i])) return 0;
  return (!is_new_game_only(i) || rules_editable == 'Y');
  }

static Int32 is_shown(Int32 i) {
  if (!is_gameplay_change(i)) return 1;
  return (i - FIRST_CHANGE >= changes_top && i - FIRST_CHANGE < changes_top + CHANGES_SHOWN);
  }

/* Scrolls the gameplay changes until row i shows. Returns 1 when they moved. */
static Int32 scroll_to(Int32 i) {
  Int32 row;
  if (is_shown(i)) return 0;
  row = i - FIRST_CHANGE;
  if (row < changes_top) changes_top = row;
    else changes_top = row - CHANGES_SHOWN + 1;
  return 1;
  }

static Int32 option_line(Int32 i) {
  if (i < NUM_RULES) return RULES_LINE + 1 + i;
  if (i < FIRST_CHANGE) return PREFS_LINE + 1 + i - NUM_RULES;
  return CHANGES_LINE + 1 + i - FIRST_CHANGE - changes_top;
  }

static Int32 option_column(Int32 i) {
  return is_gameplay_change(i) ? RIGHT_COLUMN : LEFT_COLUMN;
  }

/* The cursor visits every row of its side, wrapping at either end; a greyed row shows its help but cannot change */
static Int32 next_option(Int32 i, Int32 dir) {
  Int32 side;
  side = option_side(i);
  i += dir;
  if (i < side_first(side)) i = side_end(side) - 1;
  if (i >= side_end(side)) i = side_first(side);
  return i;
  }

static Int32 first_editable(char rules_editable) {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++)
    if (is_editable(i, rules_editable)) return i;
  return 0;
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

/* The label is padded to the value column, so a scrolled row covers the one it replaces */
static void draw_option(Int32 i, char selected, char rules_editable) {
  char  label[32];
  char  text[16];
  Int32 line;
  Int32 column;
  if (!is_shown(i)) return;
  line = option_line(i);
  column = option_column(i);

  textcolor(15);
  gotoxy(column + ARROW_INDENT, line); txt_write((selected == 'Y') ? "-->" : "   ");

  textcolor(is_editable(i, rules_editable) ? 2 : 8);
  sprintf(label, "%-*s", VALUE_INDENT - LABEL_INDENT - 1, option_rows[i].label);
  gotoxy(column + LABEL_INDENT, line); txt_write(label);

  textcolor(is_editable(i, rules_editable) ? 14 : 8);
  format_value(&option_rows[i], text);
  gotoxy(column + VALUE_INDENT, line); txt_write(text);
  }

/* Marks gameplay changes scrolled out of sight above or below the column */
static void draw_more() {
  textcolor(8);
  gotoxy(RIGHT_COLUMN + VALUE_INDENT, CHANGES_LINE);
  txt_write((changes_top > 0) ? "more ^" : "      ");
  gotoxy(RIGHT_COLUMN + VALUE_INDENT, CHANGES_LINE + CHANGES_SHOWN + 1);
  txt_write((changes_top + CHANGES_SHOWN < NUM_CHANGES) ? "more v" : "      ");
  }

static void draw_options(char rules_editable) {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) draw_option(i, 'N', rules_editable);
  draw_more();
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
  gotoxy(13, 24); txt_write("(8/2) Select   (4/6) Change   (TAB) Side   (ENTER) Done");
  }

static void edit_options(char* title, char rules_editable) {
  Int32 i;
  Int32 dir;
  Int32 side_row[2];
  char  key;
  changes_top = 0;
  draw_screen(title, rules_editable);
  i = first_editable(rules_editable);

  /* TAB returns to the row last selected on the other side */
  side_row[SIDE_LEFT] = i;
  side_row[SIDE_RIGHT] = FIRST_CHANGE;

  key = 0;
  while (key != 13 && key != 27) {
    draw_option(i, 'Y', rules_editable);
    draw_help(i);
    key = inkey();
    draw_option(i, 'N', rules_editable);
    if (key == '8') i = next_option(i, -1);
    if (key == '2') i = next_option(i, 1);
    if (key == 9 && NUM_CHANGES > 0) {
      side_row[option_side(i)] = i;
      i = side_row[1 - option_side(i)];
      }
    if (scroll_to(i)) draw_options(rules_editable);

    dir = 0;
    if (key == '4' || key == '-') dir = -1;
    if (key == '6' || key == '+' || key == '=') dir = 1;
    if (dir != 0 && is_editable(i, rules_editable)) {
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

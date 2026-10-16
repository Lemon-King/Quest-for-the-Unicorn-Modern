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
#define NUM_PREFS      22
#define NUM_CHANGES    67
#define FIRST_CHANGE   (NUM_RULES + NUM_PREFS)
#define NUM_OPTIONS    (FIRST_CHANGE + NUM_CHANGES)

#define SIDE_LEFT       0
#define SIDE_RIGHT      1

#define NUM_GROUPS      3
#define GROUP_RULES     0
#define GROUP_PREFS     1
#define GROUP_CHANGES   2

#define RULES_LINE      2
#define PREFS_LINE     11
#define CHANGES_LINE    2
#define MORE_LINE      23     /* "more v" for a group that scrolls; its rows end on the line above */
#define HELP_LINE      24
#define KEYS_LINE      25
#define LEFT_COLUMN     3
#define RIGHT_COLUMN   42
#define ARROW_INDENT    2
#define LABEL_INDENT    6
#define VALUE_INDENT   31
#define VALUE_WIDTH     7     /* the column after it holds a scrolling group's bar */

/* units follows a number's value; strict is a switch's third value, after On, and extra a fourth, after Strict, which
   needs names; names replaces Off, On, Strict and the fourth as a switch's value names; standard is a number's or
   mode's own default, and initial the value a new game starts with, after unicorn.cfg and the command line; config
   names the unicorn.cfg key of a number or switch kept there instead of in the save, a switch by its value's name;
   apply puts a new value into effect; key names a gameplay change in the save; start is the value a new game starts a
   gameplay change at, when not On or Strict; needs is a preference that must be On for this one to change */
typedef struct {
  char   label[24];
  char   help[80];
  char   units[4];
  Int32  kind;
  char*  flag;
  char   on;
  char   off;
  char   strict;
  char   extra;
  char*  names[4];
  char   casual;
  Int32* number;
  Int32  min;
  Int32  max;
  Int32  step;
  Int32  standard;
  Int32  initial;
  char*  config;
  void   (*apply)();
  char*  key;
  char   start;
  char*  needs;
  } OPTION_ROW;

/* A group of rows under its heading, from option first; its rows scroll when more of them than fit */
typedef struct {
  char*       heading;
  Int32       first;
  Int32       column;
  SCROLL_LIST rows;
  } OPTION_GROUP;

static OPTION_GROUP option_groups[NUM_GROUPS] = {
  { .heading = "Game Rules",       .first = 0,            .column = LEFT_COLUMN,
    .rows = { .count = NUM_RULES,   .shown = NUM_RULES,
              .line  = RULES_LINE,  .more_column = LEFT_COLUMN + VALUE_INDENT } },
  { .heading = "Preferences",      .first = NUM_RULES,    .column = LEFT_COLUMN,
    .rows = { .count = NUM_PREFS,   .shown = MORE_LINE - PREFS_LINE - 1,
              .line  = PREFS_LINE,  .more_column = LEFT_COLUMN + VALUE_INDENT } },
  { .heading = "Gameplay Changes", .first = FIRST_CHANGE, .column = RIGHT_COLUMN,
    .rows = { .count = NUM_CHANGES, .shown = MORE_LINE - CHANGES_LINE - 1,
              .line  = CHANGES_LINE, .more_column = RIGHT_COLUMN + VALUE_INDENT } }
  };

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
  { .label = "Dungeon Scaling",
    .help  = "Monsters in dungeons scale to your level",
    .kind  = OPT_SWITCH, .flag = &dungeon_scaling, .on = 'Y', .off = 'N' },
  { .label = "Monster Count",
    .help  = "Spawn size for monsters or monster groups, from 10 to 60",
    .kind  = OPT_NUMBER, .number = &monster_count, .min = 10, .max = 60, .step = 5 },
  { .label = "Arena Combat",
    .help  = "On: Multi-monster arena combat | Off: only bosses",
    .kind  = OPT_SWITCH, .flag = &no_arena, .on = 'N', .off = 'Y' },
  { .label = "Full Party Combat",
    .help  = "Monsters and party members take full damage",
    .kind  = OPT_SWITCH, .flag = &party_combat_mode, .on = 'F', .off = 'R' },
  { .label = "Auto-Target",
    .help  = "Target the nearest monster automatically",
    .kind  = OPT_SWITCH, .flag = &auto_target, .on = 'Y', .off = 'N' },
  { .label = "Auto-Open Doors",
    .help  = "Doors open automatically",
    .kind  = OPT_SWITCH, .flag = &auto_open_doors, .on = 'Y', .off = 'N' },
  { .label = "Auto-Search Walls",
    .help  = "Search walls automatically",
    .kind  = OPT_SWITCH, .flag = &auto_bump_walls, .on = 'Y', .off = 'N' },
  { .label = "Auto-Pickup Gold",
    .help  = "Pick up gold automatically",
    .kind  = OPT_SWITCH, .flag = &auto_pickup_gold, .on = 'Y', .off = 'N' },
  { .label = "Auto-Pickup Torches",
    .help  = "Pick up torches automatically",
    .kind  = OPT_SWITCH, .flag = &auto_pickup_torch, .on = 'Y', .off = 'N' },
  { .label = "Improved Interface",
    .help  = "Cursor menus, item cards, rarity colours, centred scrolls and the TAB menu",
    .kind  = OPT_SWITCH, .flag = &improved_interface, .on = 'Y', .off = 'N' },
  { .label = "Easy Spells",
    .help  = "Pick your spell from a list in the TAB menu, instead of typing its name",
    .kind  = OPT_SWITCH, .flag = &easy_spells, .on = 'Y', .off = 'N', .needs = &improved_interface },
  { .label = "Improved Status",
    .help  = "Status row under your stats: grey letters, coloured letters, or icons",
    .kind  = OPT_SWITCH, .flag = &improved_status, .on = 'C', .off = 'N', .strict = 'I',
    .names = { "Off", "Color", "Icons" } },
  { .label = "Floor Item Names",
    .help  = "Shows the name of the item you stand on, in the bottom right",
    .kind  = OPT_SWITCH, .flag = &floor_item_names, .on = 'Y', .off = 'N' },
  { .label = "Improved Items",
    .help  = "Card's rarity colours, * marks unidentified items, common drops identified",
    .kind  = OPT_SWITCH, .flag = &improved_items, .on = 'Y', .off = 'N' },
  { .label = "Enhanced Sprites",
    .help  = "Improves tile display for Sprites",
    .kind  = OPT_SWITCH, .flag = &enhanced_sprites, .on = 'Y', .off = 'N' },
  { .label = "Unique Class Sprites",
    .help  = "Show your class's own figure where there is one: cavalier, cleric or mage",
    .kind  = OPT_SWITCH, .flag = &class_sprites, .on = 'Y', .off = 'N' },
  { .label = "Show Mount",
    .help  = "On the overworld, the mount you ride shows in place of your own sprite",
    .kind  = OPT_SWITCH, .flag = &show_mount, .on = 'Y', .off = 'N' },
  { .label = "Seamless World Map",
    .help  = "Overworld scrolls across region edges; minimap follows you, or per Region",
    .kind  = OPT_SWITCH, .flag = &seamless_world_map, .on = 'Y', .off = 'N', .strict = 'R',
    .names = { "Off", "On", "Region" } },
  { .label = "Follow Camera",
    .help  = "The view follows you once you leave its middle fifth, stopping at map edges",
    .kind  = OPT_SWITCH, .flag = &follow_camera, .on = 'Y', .off = 'N' },
  { .label = "World Map",
    .help  = "What M shows on the upland: the world you have found, in colours or tile art",
    .kind  = OPT_SWITCH, .flag = &world_map, .on = 'C', .off = 'N', .strict = 'A',
    .names = { "Off", "Color", "Art" }, .config = "WorldMap" },
  { .label = "Gothic Scrolls",
    .help  = "Writes scrolls in Gothic instead of the hand-drawn scroll letters",
    .kind  = OPT_SWITCH, .flag = &gothic_scrolls, .on = 'Y', .off = 'N' },
  { .label = "Color Palette",
    .help  = "Text and map colours: X11, as on Windows, or the DOS (CGA/EGA/VGA) colours",
    .kind  = OPT_SWITCH, .flag = &color_palette, .on = 'D', .off = 'X',
    .names = { "X11", "DOS" }, .config = "ColorPalette" },
  { .label = "Ranged Delay (ms)",
    .help  = "Ranged attack flight delay",
    .kind  = OPT_NUMBER, .number = &delay_ranged, .min = 0, .max = 1000, .step = 5 },
  { .label = "Hit Delay (ms)",
    .help  = "Successful hit effect delay",
    .kind  = OPT_NUMBER, .number = &delay_hit, .min = 0, .max = 1000, .step = 25 },
  { .label = "Trap Delay (ms)",
    .help  = "Trap effects delay",
    .kind  = OPT_NUMBER, .number = &delay_trap, .min = 0, .max = 1000, .step = 25 },
  { .label = "Music Volume",
    .help  = "Music Volume from 0% to 100%", .units = "%",
    .kind  = OPT_NUMBER, .number = &music_volume, .min = 0, .max = 100, .step = 10,
    .config = "MusicVolume", .apply = set_music_volume },
  { .label = "World Seed",
    .help  = "New worlds pick from 2 billion dungeon layouts, not 32,767",
    .kind  = OPT_SWITCH, .flag = &change_world_seed, .on = 'Y', .off = 'N', .key = "WorldSeed" },
  { .label = "New Game Dungeon",
    .help  = "A new character's first visit to the Temple of the Forest builds it",
    .kind  = OPT_SWITCH, .flag = &change_new_game_dungeon, .on = 'Y', .off = 'N', .key = "NewGameDungeon" },
  { .label = "Quest State",
    .help  = "The compass keeps to dungeons you entered or heard of after a load too",
    .kind  = OPT_SWITCH, .flag = &change_quest_state, .on = 'Y', .off = 'N', .key = "QuestState" },
  { .label = "Compass",
    .help  = "The compass points the right way to and from the eastern regions",
    .kind  = OPT_SWITCH, .flag = &change_compass, .on = 'Y', .off = 'N', .key = "Compass" },
  { .label = "Lost Entrance",
    .help  = "One dungeon whose entrance was lost can be entered, and the compass finds it",
    .kind  = OPT_SWITCH, .flag = &change_valley_of_fear, .on = 'Y', .off = 'N', .key = "ValleyOfFear" },
  { .label = "Town Completion",
    .help  = "Entering a town marks it complete, so the Completionist trophy can be won",
    .kind  = OPT_SWITCH, .flag = &change_town_completion, .on = 'Y', .off = 'N', .key = "TownCompletion" },
  { .label = "Town Trophy",
    .help  = "Completionist counts dungeons only; visiting every town earns Sightseer",
    .kind  = OPT_SWITCH, .flag = &change_town_trophy, .on = 'Y', .off = 'N', .key = "TownTrophy" },
  { .label = "Free Upland Keys",
    .help  = "Help, records, options, unused keys and blocked steps take no upland time",
    .kind  = OPT_SWITCH, .flag = &change_free_upland_keys, .on = 'Y', .off = 'N', .key = "FreeUplandKeys" },
  { .label = "Upland Quit",
    .help  = "Q on the overworld asks before quitting, as it does in dungeons",
    .kind  = OPT_SWITCH, .flag = &change_upland_quit, .on = 'Y', .off = 'N', .key = "UplandQuit" },
  { .label = "Explored Land",
    .help  = "A game saved on the upland keeps the land you explored in your region",
    .kind  = OPT_SWITCH, .flag = &change_explored_land, .on = 'Y', .off = 'N', .key = "ExploredLand" },
  { .label = "Hunting Skill",
    .help  = "Hunting on the upland improves with Scouting, the skill it trains, not Mace",
    .kind  = OPT_SWITCH, .flag = &change_hunting_skill, .on = 'Y', .off = 'N', .key = "HuntingSkill" },
  { .label = "Ship Purchase",
    .help  = "A bought ship is yours at once, so a second one cannot be bought",
    .kind  = OPT_SWITCH, .flag = &change_ship_purchase, .on = 'Y', .off = 'N', .key = "ShipPurchase" },
  { .label = "Ship Improvements",
    .help  = "Ships sail into and out of port towns, stay in harbours, never cover a town",
    .kind  = OPT_SWITCH, .flag = &change_ship_improvements, .on = 'Y', .off = 'N', .key = "ShipImprovements" },
  { .label = "Travel Saves",
    .help  = "Saves keep a relic's destinations without losing lore or your position",
    .kind  = OPT_SWITCH, .flag = &change_tardis_saves, .on = 'Y', .off = 'N', .key = "TardisSaves" },
  { .label = "Travel Descriptions",
    .help  = "Destination descriptions stop at 31 characters instead of overrunning memory",
    .kind  = OPT_SWITCH, .flag = &change_tardis_descriptions, .on = 'Y', .off = 'N', .key = "TardisDescriptions" },
  { .label = "Sleep Traps",
    .help  = "Sleep traps put you to sleep instead of never springing",
    .kind  = OPT_SWITCH, .flag = &change_sleep_traps, .on = 'Y', .off = 'N', .key = "SleepTraps" },
  { .label = "Words of Power",
    .help  = "Rising from a first level leaves it; sinking does nothing in a town",
    .kind  = OPT_SWITCH, .flag = &change_words_of_power, .on = 'Y', .off = 'N', .key = "WordsOfPower" },
  { .label = "Final Dungeon Exit",
    .help  = "Leaving the final dungeon ends its rules, so later dungeons are normal again",
    .kind  = OPT_SWITCH, .flag = &change_gateway_exit, .on = 'Y', .off = 'N', .key = "GatewayExit" },
  { .label = "Final Boss Spawns",
    .help  = "The final boss waits on the fifth level of the final dungeon",
    .kind  = OPT_SWITCH, .flag = &change_eshter_spawns, .on = 'Y', .off = 'N', .key = "EshterSpawns" },
  { .label = "Den of Horror Guards",
    .help  = "Guards protect a relic on the bottom level of the Den of Horror",
    .kind  = OPT_SWITCH, .flag = &change_dalek_spawns, .on = 'Y', .off = 'N', .key = "DalekSpawns" },
  { .label = "Castle Treasure",
    .help  = "One treasure at a time in the final boss's castle; Strict, none if owned",
    .kind  = OPT_SWITCH, .flag = &change_nether_core, .on = 'Y', .off = 'N', .strict = 'S', .key = "NetherCore" },
  { .label = "Sun Stone",
    .help  = "Returning by Sun Stone no longer saves the town over the level above",
    .kind  = OPT_SWITCH, .flag = &change_sun_stone, .on = 'Y', .off = 'N', .key = "SunStone" },
  { .label = "Sun Stone Final Dungeon",
    .help  = "A Sun Stone used in the final dungeon brings you back to it",
    .kind  = OPT_SWITCH, .flag = &change_sun_stone_gateway, .on = 'Y', .off = 'N', .key = "SunStoneGateway" },
  { .label = "Sun Stone Fixes",
    .help  = "The Sun Stone returns to the dungeon it holds; records only dungeon levels",
    .kind  = OPT_SWITCH, .flag = &change_sun_stone_fixes, .on = 'Y', .off = 'N', .key = "SunStoneFixes" },
  { .label = "Elemental Weapon Damage",
    .help  = "Equal physical and elemental weapon damage no longer deals 0",
    .kind  = OPT_SWITCH, .flag = &change_weapon_damage, .on = 'Y', .off = 'N', .key = "ElementalWeaponDamage" },
  { .label = "Ranged Damage",
    .help  = "Arrows and thrown weapons roll one crit and count their damage once",
    .kind  = OPT_SWITCH, .flag = &change_ranged_damage, .on = 'Y', .off = 'N', .key = "RangedDamage" },
  { .label = "Thrown Weapons",
    .help  = "Thrown rocks and stars no longer crash the game when they land on an item",
    .kind  = OPT_SWITCH, .flag = &change_thrown_weapons, .on = 'Y', .off = 'N', .key = "ThrownWeapons" },
  { .label = "Poison Kills",
    .help  = "A creature killed by poison leaves the map where it died, not under you",
    .kind  = OPT_SWITCH, .flag = &change_poison_kills, .on = 'Y', .off = 'N', .key = "PoisonKills" },
  { .label = "Summon Combat",
    .help  = "Monsters fight your summoned creature properly, and it can find you",
    .kind  = OPT_SWITCH, .flag = &change_summon_combat, .on = 'Y', .off = 'N', .key = "SummonCombat" },
  { .label = "Party Joining",
    .help  = "New party members get their party attack, defense and mana",
    .kind  = OPT_SWITCH, .flag = &change_party_joining, .on = 'Y', .off = 'N', .key = "PartyJoining" },
  { .label = "Teleport Spell",
    .help  = "Teleport moves you across a dungeon level instead of doing nothing",
    .kind  = OPT_SWITCH, .flag = &change_teleport_spell, .on = 'Y', .off = 'N', .key = "TeleportSpell" },
  { .label = "Blink Only Blinks",
    .help  = "Blink no longer also casts Protection",
    .kind  = OPT_SWITCH, .flag = &change_blink_only, .on = 'Y', .off = 'N', .key = "BlinkOnlyBlinks" },
  { .label = "Blink Landing",
    .help  = "Blink leaves you where it lands, never in a wall near the map's edge",
    .kind  = OPT_SWITCH, .flag = &change_blink_landing, .on = 'Y', .off = 'N', .key = "BlinkLanding" },
  { .label = "Spell Targets",
    .help  = "Blind, Sleep, Drain and reflected spells affect the creature they hit",
    .kind  = OPT_SWITCH, .flag = &change_spell_targets, .on = 'Y', .off = 'N', .key = "SpellTargets" },
  { .label = "Blast Spells",
    .help  = "Nuke, Nova and other blasts respect anti-magic and only anger friends",
    .kind  = OPT_SWITCH, .flag = &change_blast_spells, .on = 'Y', .off = 'N', .key = "BlastSpells" },
  { .label = "Spell Kills",
    .help  = "Spell kills drop loot, count as kills and share exp like other kills",
    .kind  = OPT_SWITCH, .flag = &change_spell_kills, .on = 'Y', .off = 'N', .key = "SpellKills" },
  { .label = "Scroll of Charging",
    .help  = "Scroll of Charging restores charges instead of freezing the game",
    .kind  = OPT_SWITCH, .flag = &change_scroll_charging, .on = 'Y', .off = 'N', .key = "ScrollOfCharging" },
  { .label = "Mind Shield",
    .help  = "Potions and scrolls of Mind Shield wear off like the other resistances",
    .kind  = OPT_SWITCH, .flag = &change_mind_shield, .on = 'Y', .off = 'N', .key = "MindShield" },
  { .label = "Arena Assassins",
    .help  = "Assassins keep their level in arena combat instead of restarting at 1",
    .kind  = OPT_SWITCH, .flag = &change_arena_assassins, .on = 'Y', .off = 'N', .key = "ArenaAssassins" },
  { .label = "Arena Monk Fear",
    .help  = "Monks cannot be frightened in the arena from level 11, as in dungeons",
    .kind  = OPT_SWITCH, .flag = &change_arena_monk_fear, .on = 'Y', .off = 'N', .key = "ArenaMonkFear" },
  { .label = "Arena Timers",
    .help  = "Resistances, blindness, light and polymorph wear off during arena fights",
    .kind  = OPT_SWITCH, .flag = &change_arena_timers, .on = 'Y', .off = 'N', .key = "ArenaTimers" },
  { .label = "Arena Decorations",
    .help  = "Trees and rocks in the arena never land on a monster or party member",
    .kind  = OPT_SWITCH, .flag = &change_arena_decorations, .on = 'Y', .off = 'N', .key = "ArenaDecorations" },
  { .label = "Arena Exit",
    .help  = "ENTER or ESC leaves the arena once every monster is dead",
    .kind  = OPT_SWITCH, .flag = &change_arena_exit, .on = 'Y', .off = 'N', .key = "ArenaExit" },
  { .label = "Arena Held Keys",
    .help  = "Keys held as a fight starts or ends no longer make the game wait for another",
    .kind  = OPT_SWITCH, .flag = &change_arena_held_keys, .on = 'Y', .off = 'N', .key = "ArenaHeldKeys" },
  { .label = "Safe Arena Loot",
    .help  = "Items left on a dungeon arena's floor lie around you when the fight ends",
    .kind  = OPT_SWITCH, .flag = &change_safe_arena_loot, .on = 'Y', .off = 'N', .key = "SafeArenaLoot" },
  { .label = "Rarity Fix",
    .help  = "Pickups count rarity alike: A as g and its card, B as G, C as B with 5 ultra",
    .kind  = OPT_SWITCH, .flag = &change_rarity_fix, .on = 'A', .off = 'N', .strict = 'B', .extra = 'C',
    .names = { "Off", "Band A", "Band B", "Band C" }, .key = "RarityFix", .start = 'A' },
  { .label = "Unified Pickup",
    .help  = "Packing with G adds cards to your collection and completes quests, as g does",
    .kind  = OPT_SWITCH, .flag = &change_unified_pickup, .on = 'Y', .off = 'N', .key = "UnifiedPickup" },
  { .label = "Floor Items",
    .help  = "Packed items and arena loot free their spots, and a full floor drops nothing",
    .kind  = OPT_SWITCH, .flag = &change_floor_items, .on = 'Y', .off = 'N', .key = "FloorItems" },
  { .label = "Lore Books",
    .help  = "G and P on a lore book do nothing, instead of copying a Portable Hole item",
    .kind  = OPT_SWITCH, .flag = &change_lore_books, .on = 'Y', .off = 'N', .key = "LoreBooks" },
  { .label = "Pack Weight",
    .help  = "The item in the last pack slot counts toward the weight you carry",
    .kind  = OPT_SWITCH, .flag = &change_pack_weight, .on = 'Y', .off = 'N', .key = "PackWeight" },
  { .label = "Possessions",
    .help  = "Items you used up or threw away no longer count as still yours",
    .kind  = OPT_SWITCH, .flag = &change_possessions, .on = 'Y', .off = 'N', .key = "Possessions" },
  { .label = "Item Deck",
    .help  = "The item deck never deals the broken \"EOL\" item",
    .kind  = OPT_SWITCH, .flag = &change_item_deck, .on = 'Y', .off = 'N', .key = "ItemDeck" },
  { .label = "Tripped Card",
    .help  = "The Deck of Many Things' Tripped card costs your own class's hit die",
    .kind  = OPT_SWITCH, .flag = &change_tripped_card, .on = 'Y', .off = 'N', .key = "TrippedCard" },
  { .label = "Manual of Bodily Health",
    .help  = "The Manual of Bodily Health no longer also uses the Crown of Might",
    .kind  = OPT_SWITCH, .flag = &change_bodily_health, .on = 'Y', .off = 'N', .key = "BodilyHealth" },
  { .label = "Shaman Legendaries",
    .help  = "Ayres' Mythical items found by a Shaman get the Cleric's bonuses and a mace",
    .kind  = OPT_SWITCH, .flag = &change_shaman_legendaries, .on = 'Y', .off = 'N', .key = "ShamanLegendaries" },
  { .label = "Last Bite",
    .help  = "Eating the last bite of a food with e counts as a meal, as with u",
    .kind  = OPT_SWITCH, .flag = &change_last_bite, .on = 'Y', .off = 'N', .key = "LastBite" },
  { .label = "Meat and Potatoes",
    .help  = "Meat and Potatoes counts meat eaten, and meat counts once as a meal",
    .kind  = OPT_SWITCH, .flag = &change_meat_potatoes, .on = 'Y', .off = 'N', .key = "MeatAndPotatoes" },
  { .label = "Village Shops",
    .help  = "Village merchants sell; their weapons and armour are only levels 1 and 2",
    .kind  = OPT_SWITCH, .flag = &change_village_shops, .on = 'Y', .off = 'N', .key = "VillageShops" },
  { .label = "Shop Boots and Wands",
    .help  = "Armor shops sell boots, and magic shops sell wands",
    .kind  = OPT_SWITCH, .flag = &change_shop_boots_wands, .on = 'Y', .off = 'N', .key = "ShopBootsAndWands" },
  { .label = "Shop Dagger",
    .help  = "Weapon shops sell the Dagger once, not again from the item table's first row",
    .kind  = OPT_SWITCH, .flag = &change_shop_dagger, .on = 'Y', .off = 'N', .key = "ShopDagger" },
  { .label = "Platinum Dragon Mount",
    .help  = "A Platinum Dragon bought at the stable is a flying mount like other dragons",
    .kind  = OPT_SWITCH, .flag = &change_platinum_dragon_mount, .on = 'Y', .off = 'N', .key = "PlatinumDragonMount" },
  { .label = "Temple Uncurse",
    .help  = "Leaving a temple's uncurse without picking an item no longer crashes",
    .kind  = OPT_SWITCH, .flag = &change_temple_uncurse, .on = 'Y', .off = 'N', .key = "TempleUncurse" },
  { .label = "Library Text",
    .help  = "Town libraries show messages in plain text instead of enciphered",
    .kind  = OPT_SWITCH, .flag = &change_library_text, .on = 'Y', .off = 'N', .key = "LibraryText" },
  { .label = "Lore Search",
    .help  = "Library searches show and learn the message that matched, not the next",
    .kind  = OPT_SWITCH, .flag = &change_lore_search, .on = 'Y', .off = 'N', .key = "LoreSearch" },
  { .label = "House Buying",
    .help  = "Owning a house no longer stops you buying the others",
    .kind  = OPT_SWITCH, .flag = &change_house_buying, .on = 'Y', .off = 'N', .key = "HouseBuying" },
  { .label = "House Storage",
    .help  = "All 500 house slots show, and leaving a full house keeps the item In Air",
    .kind  = OPT_SWITCH, .flag = &change_house_storage, .on = 'Y', .off = 'N', .key = "HouseStorage" },
  { .label = "No Negative Amounts",
    .help  = "Negative bank, house and dice amounts count as 0 instead of adding gold",
    .kind  = OPT_SWITCH, .flag = &change_negative_amounts, .on = 'Y', .off = 'N', .key = "NoNegativeAmounts" }
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

static Int32 is_held_by_need(OPTION_ROW* row) {
  return (row->needs != NULL && *row->needs != 'Y');
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

/* The groups TAB moves between: rules, preferences and gameplay changes */
static Int32 option_group(Int32 i) {
  if (i < NUM_RULES) return GROUP_RULES;
  if (i < FIRST_CHANGE) return GROUP_PREFS;
  return GROUP_CHANGES;
  }

static Int32 is_new_game_only(Int32 i) {
  return (i < NUM_RULES || is_gameplay_change(i));
  }

static Int32 is_editable(Int32 i, char rules_editable) {
  if (is_held_by_mode(&option_rows[i]) || is_held_by_need(&option_rows[i])) return 0;
  return (!is_new_game_only(i) || rules_editable == 'Y');
  }

static Int32 is_shown(Int32 i) {
  OPTION_GROUP* group;

  group = &option_groups[option_group(i)];

  return list_shows(&group->rows, i - group->first);
  }

/* Scrolls row i's group until it shows. Returns 1 when the group moved. */
static Int32 scroll_to(Int32 i) {
  OPTION_GROUP* group;

  group = &option_groups[option_group(i)];

  return list_scroll_to(&group->rows, i - group->first);
  }

static Int32 option_line(Int32 i) {
  OPTION_GROUP* group;

  group = &option_groups[option_group(i)];

  return list_line(&group->rows, i - group->first);
  }

static Int32 option_column(Int32 i) {
  return option_groups[option_group(i)].column;
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

/* A switch is on at On, and at its strict and extra values if it has them */
static Int32 is_switch_on(OPTION_ROW* row, char flag) {
  if (row->strict != 0 && flag == row->strict) return 1;
  if (row->extra != 0 && flag == row->extra) return 1;

  return (flag == row->on);
  }

static char* switch_name(OPTION_ROW* row, char flag) {
  static char* standard_names[3] = { "Off", "On", "Strict" };
  char**       names;

  names = (row->names[0] != NULL) ? row->names : standard_names;

  if (flag == row->on) return names[1];
  if (row->strict != 0 && flag == row->strict) return names[2];
  if (row->extra != 0 && flag == row->extra) return names[3];

  return names[0];
  }

/* A row kept in unicorn.cfg: a number as itself, a switch as its value's name */
static void save_config_row(OPTION_ROW* row) {
  if (row->kind == OPT_SWITCH) save_config_text("unicorn",row->config,switch_name(row,*row->flag));
    else save_config_value("unicorn",row->config,*row->number);
  }

/* Sets the switch unicorn.cfg keeps under key to the value called name, as save_config_row() writes it; any other
   name leaves it as it is */
void set_config_switch(char* key,char* name) {
  OPTION_ROW* row;
  Int32       i;

  while (*name == ' ') name++;
  for (i=0; i<NUM_OPTIONS; i++) {
    row = &option_rows[i];
    if (row->kind != OPT_SWITCH || row->config == NULL || _stricmp(row->config,key) != 0) continue;

    if (_stricmp(name,switch_name(row,row->off)) == 0) *row->flag = row->off;
    if (_stricmp(name,switch_name(row,row->on)) == 0) *row->flag = row->on;
    if (row->strict != 0 && _stricmp(name,switch_name(row,row->strict)) == 0) *row->flag = row->strict;
    if (row->extra != 0 && _stricmp(name,switch_name(row,row->extra)) == 0) *row->flag = row->extra;
    }
  }

/* Steps a switch through Off and On, then Strict and the fourth value when it has them, wrapping either way */
static char next_switch_value(OPTION_ROW* row, Int32 dir) {
  char  values[4];
  Int32 count;
  Int32 i;

  values[0] = row->off;
  values[1] = row->on;
  values[2] = row->strict;
  values[3] = row->extra;
  count = 2;
  if (row->strict != 0) count++;
  if (row->extra != 0) count++;

  i = 0;
  while (i < count && values[i] != *row->flag) i++;
  if (i == count) i = 0;

  return values[(i + dir + count) % count];
  }

static void format_value(OPTION_ROW* row, char* text) {
  char flag;
  char number[16];
  switch (row->kind) {
    case OPT_MODE:   sprintf(text, "%-*s", VALUE_WIDTH, mode_name(*row->number)); break;
    case OPT_SWITCH: flag = is_held_by_mode(row) ? row->casual : *row->flag;
                     sprintf(text, "%-*s", VALUE_WIDTH, switch_name(row, flag));
                     break;
    case OPT_NUMBER: sprintf(number, "%d%s", *row->number, row->units);
                     sprintf(text, "%-*s", VALUE_WIDTH, number);
                     break;
    }
  }

static Int32 option_value(OPTION_ROW* row) {
  if (row->kind == OPT_SWITCH) return *row->flag;

  return *row->number;
  }

/* A switch is blue On or Strict and grey Off; a number or mode is yellow at the game's own default and blue when
   changed */
static Int32 value_color(OPTION_ROW* row) {
  if (row->kind == OPT_SWITCH) return is_switch_on(row, *row->flag) ? CLR_LIGHTBLUE : CLR_GRAY;
  if (*row->number == row->standard) return CLR_YELLOW;

  return CLR_LIGHTBLUE;
  }

static void change_option(OPTION_ROW* row, Int32 dir) {
  switch (row->kind) {
    case OPT_MODE:
         *row->number = mode_values[(mode_index(*row->number) + dir + NUM_MODES) % NUM_MODES];
         break;
    case OPT_SWITCH:
         *row->flag = next_switch_value(row, dir);
         break;
    case OPT_NUMBER:
         *row->number += dir * row->step;
         if (*row->number < row->min) *row->number = row->min;
         if (*row->number > row->max) *row->number = row->max;
         break;
    }

  if (row->apply != NULL) row->apply();
  if (row->config != NULL) save_config_row(row);
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

  draw_list_arrow(&option_groups[option_group(i)].rows, column + ARROW_INDENT, line, selected == 'Y');

  textcolor(is_editable(i, rules_editable) ? 2 : 8);
  sprintf(label, "%-*s", VALUE_INDENT - LABEL_INDENT - 1, option_rows[i].label);
  gotoxy(column + LABEL_INDENT, line);
  txt_write(label);

  textcolor(is_editable(i, rules_editable) ? value_color(&option_rows[i]) : 8);
  format_value(&option_rows[i], text);
  gotoxy(column + VALUE_INDENT, line);
  txt_write(text);
  }

static void draw_options(char rules_editable) {
  Int32 i;
  Int32 g;

  for (i=0; i<NUM_OPTIONS; i++) draw_option(i, 'N', rules_editable);
  for (g=0; g<NUM_GROUPS; g++) draw_list_more(&option_groups[g].rows);
  }

static void draw_help(Int32 i) {
  char text[80];
  sprintf(text, "%-76s", option_rows[i].help);
  textcolor(7);
  gotoxy(3, HELP_LINE);
  txt_write(text);
  }

static void draw_screen(char* title, char rules_editable) {
  Int32 g;

  textmode(3);
  write_centred(1,12,title);

  textcolor(11);
  for (g=0; g<NUM_GROUPS; g++) {
    gotoxy(option_groups[g].column, option_groups[g].rows.line);
    txt_write(option_groups[g].heading);
    }
  draw_options(rules_editable);

  textcolor(3);
  gotoxy(13, KEYS_LINE);
  txt_write("(8/2) Select   (4/6) Change   (TAB) Group   (ENTER) Done");
  }

static void edit_options(char* title, char rules_editable) {
  Int32 i;
  Int32 dir;
  Int32 group;
  Int32 group_row[NUM_GROUPS];
  char  key;
  char  palette;

  for (group=0; group<NUM_GROUPS; group++) option_groups[group].rows.top = 0;
  draw_screen(title, rules_editable);
  i = first_editable(rules_editable);

  /* TAB moves to the next group, back to the row last selected there */
  for (group=0; group<NUM_GROUPS; group++) group_row[group] = option_groups[group].first;
  group_row[option_group(i)] = i;

  key = 0;
  while (key != 13 && key != 27) {
    draw_option(i, 'Y', rules_editable);
    draw_help(i);
    key = inkey();
    draw_option(i, 'N', rules_editable);
    if (key == '8') i = next_option(i, -1);
    if (key == '2') i = next_option(i, 1);
    if (key == 9) {
      group_row[option_group(i)] = i;
      group = (option_group(i) + 1) % NUM_GROUPS;
      if (option_groups[group].rows.count == 0) group = GROUP_RULES;
      i = group_row[group];
      }
    if (scroll_to(i)) draw_options(rules_editable);

    dir = 0;
    if (key == '4' || key == '-') dir = -1;
    if (key == '6' || key == '+' || key == '=') dir = 1;
    if (dir != 0 && is_editable(i, rules_editable)) {
      palette = color_palette;
      change_option(&option_rows[i], dir);

      /* A new palette shows only in what is drawn after it */
      if (color_palette != palette) draw_screen(title, rules_editable);
        else draw_options(rules_editable);
      }
    }
  }

/* Sets the switches Casual mode holds to their casual values */
static void apply_mode_holds() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++)
    if (is_held_by_mode(&option_rows[i])) *option_rows[i].flag = option_rows[i].casual;
  }

/* Keeps the game's own numbers and modes, set before unicorn.cfg is read, which the screens show in yellow */
void remember_built_in_defaults() {
  Int32 i;

  for (i=0; i<NUM_OPTIONS; i++)
    if (option_rows[i].kind != OPT_SWITCH) option_rows[i].standard = *option_rows[i].number;
  }

/* Keeps the values from unicorn.cfg and the command line, which pre-fill every new game. Gameplay changes start On,
   or Strict where they have it, or at their start value where they name one. */
void remember_option_defaults() {
  Int32 i;
  for (i=0; i<NUM_OPTIONS; i++) {
    if (is_gameplay_change(i) && option_rows[i].start != 0) option_rows[i].initial = option_rows[i].start;
      else if (is_gameplay_change(i) && option_rows[i].strict != 0) option_rows[i].initial = option_rows[i].strict;
      else if (is_gameplay_change(i)) option_rows[i].initial = option_rows[i].on;
      else option_rows[i].initial = option_value(&option_rows[i]);
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

/*
  int          cleared[10];
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "platform.h"

#ifdef LINK
#undef LINK
#endif

#ifdef MAIN
#define LINK
#else
#define LINK extern
#endif

#ifndef _BYTE_
typedef unsigned char byte;
#endif

#ifndef _WORD_
typedef unsigned short word;
#endif

typedef uint32_t  UInt32;
typedef int32_t   Int32;

#include "terminal.h"

#define PATH_MAX             10
#define NUM_CLASSES          13
#define NO_OF_MAPS           50
#define NO_OF_SPRITES       200
LINK Int32 NO_OF_MONSTERS;
#define ARENA_SIZE_X          15
#define ARENA_SIZE_Y          15

/* The Gateway: dungeon 99, entered at (80,78) in region 12 */
#define GATEWAY_DUNGEON       99
#define GATEWAY_REGION        12
#define GATEWAY_X             80
#define GATEWAY_Y             78
#define MESSAGE_HEIGHT        12
#define MAX_MSG               60              /* Lines the message log keeps; NUM_MSG never shows more */


/*
#define NO_OF_OBJECTS       195
#define TOTAL_OBJECTS       195
#define NO_OF_LORE           54
#define TOTAL_LORE           95
#define NO_OF_SPELLS        215
*/
#define GAME          "UNICORN"
#define VERSION        "v7.0.0"
#define RECIPE_BASE        4000
#define NPC_DIV              10

#define MAX_HALL_OF_FAME   1000
#define MAX_FINAL_SCORES  10000
#define HEAL_TIME           500

/* Background music, as in the DOS release: intro from the title on, then one song per mode */
#define MUSIC_INTRO           0
#define MUSIC_UPLAND          1
#define MUSIC_DUNGEON         2
#define MUSIC_TOWN            3

/* maps[]: index 0 repeats region 1, then regions 1-12, the towns (13-17) and Eshter's realm (18) */
#define MAP_COUNT            19

#define OF_FOR_SALE  (1 << 0)
#define OF_NO_GACHA  (1 << 1)
#define OF_UNIQUE    (1 << 2)
#define OF_QUEST     (1 << 3)
#define OF_OBJECT    (1 << 4)      /* must have item in flag2 for item to drop */
#define OF_COUNTED   (1 << 5)      /* Object counted already for stats purposes */
#define OF_IDENT     (1 << 6)      /* Object has been identified */

#define CF_MONK         (1 << 0)   /*     1  must check monk_weapon() for armor and weapons */
#define CF_CASTER       (1 << 1)   /*     2  Class selects a spell when character is created */
#define CF_NOBLE        (1 << 2)   /*     4  Gains 10,000 gp per level */
#define CF_ROGUE        (1 << 3)   /*     8  Class character becomes if they turn evil */
#define CF_CAVALIER     (1 << 4)   /*    16  Receives cavalier pay of 1,000 gp per level per year */
#define CF_PALADIN      (1 << 5)   /*    32  Palidin attributes, improved Heal command */
#define CF_HIER         (1 << 9)   /*   512  Hierophant druid */
#define CF_DRUID        (1 << 10)  /*  1024  For interactions with the druidic temple */
#define CF_INITIAL      (1 << 11)  /*  2048  Available as an initial class */

#define MS_AMAGIC             (1 << 0)
#define MS_SLEEP              (1 << 1)
#define MS_POISON             (1 << 2)
#define MS_FEAR               (1 << 3)
#define MS_FRIENDLY           (1 << 4)
#define MS_APHYSICAL          (1 << 5)
#define MS_CONFUSION          (1 << 6)
#define MS_FIRE_ATK           (1 << 7)
#define MS_COLD_ATK           (1 << 8)
#define MS_RANGED             (1 << 9)
#define MS_HEALER             (1 << 10)
#define MS_BLINK              (1 << 11)
#define MS_ELEC_ATK           (1 << 12)
#define MS_ARCN_ATK           (1 << 13)
#define MS_FLY                (1 << 14)
#define MS_MOUNT              (1 << 15)
#define MS_EQUIP              (1 << 16)
#define MS_MOVED              (1 << 17)
#define MS_TALKED             (1 << 18)
#define MS_EVADE              (1 << 19)
#define MS_ROOT               (1 << 20)
#define MS_PSIONIC            (1 << 21)
#define MS_DOT                (1 << 22)
#define MS_RFIRE              (1 << 23)
#define MS_RCOLD              (1 << 24)
#define MS_RELEC              (1 << 25)
#define MS_RARCN              (1 << 26)
#define MS_IFIRE              (1 << 27)
#define MS_ICOLD              (1 << 28)
#define MS_IELEC              (1 << 29)
#define MS_IARCN              (1 << 30)
#define MS_JOIN               (1 << 31)

#define M2_REF_PHYS           (1 << 0)
#define M2_REF_MAGIC          (1 << 1)
#define M2_REF_FIRE           (1 << 2)
#define M2_REF_COLD           (1 << 3)
#define M2_REF_ELEC           (1 << 4)
#define M2_REF_ARCN           (1 << 5)
#define M2_PACIFIED           (1 << 6)
#define M2_BOSS               (1 << 7)
#define M2_CITY               (1 << 8)                 /* Can appear in cities */
#define M2_NORND              (1 << 9)                 /* Do not allow monster as random selection */
#define M2_DRAIN              (1 << 10)                /* Chance to drain level when hits */

#define MT_HUMANOID           1
#define MT_BEAST              2
#define MT_DEMON              3
#define MT_DRAGON             4
#define MT_UNDEAD             5
#define MT_ELEMENTAL          6
#define MT_GIANT              7

#define SP_BLNK               1
#define SP_PROT               2
#define SP_TCH                3
#define SP_MSSL               4
#define SP_SLEP               6
#define SP_SPCL               7
#define SP_FEAR               8
#define SP_MSL9               9
#define SP_BLND              10
#define SP_AR25              11
#define SP_DRN               12
#define SP_AR49              13
#define SP_VLA               14
#define SP_ENCH              15
#define SP_SUMN              26

#define SK_BOW                1
#define SK_1HSWORD            2
#define SK_SPEAR              3
#define SK_MACE               4
#define SK_SCOUT              5
#define SK_LANG               6
#define SK_LORE               7
#define SK_MAGIC              8
#define SK_HEAL               9
#define SK_MED               10
#define SK_RIDE              11
#define SK_BOAT              12
#define SK_ORACLE            13
#define SK_CHARM             14
#define SK_SURVIVE           15
#define SK_COMBAT            16
#define SK_DAGGER            17
#define SK_2HSWORD           18
#define SK_AXE               19
#define SK_FLAIL             20
#define SK_CROSSBOW          21
#define SK_TRIDENT           22
#define SK_POLEAXE           23
#define SK_THROWN            24


#define O_NONE                0
#define O_EDGE                1
#define O_ARMR                2
#define O_POTN                3
#define O_RING                4
#define O_STFF                5
#define O_FOOD                6
#define O_MISC                7
#define O_SCRL                8
#define O_NECK                9
#define O_ROBE               10
#define O_BOOT               11
#define O_HELM               12
#define O_SHLD               13
#define O_BOOK               14
#define O_POLE               15
#define O_BOW                16
#define O_ONCE               17
#define O_SMWP               18
#define O_JEWL               19
#define O_PROJ               20         /* projected energy weapon */
#define O_DAGR               21         /* throwable daggers */
#define O_CRFT               22         /* Crafting items */
#define O_ALCH               23         /* Alchoholic drink */
#define O_BLNT               24         /* Blunt weapons */
#define O_WAND               25         /* Wand weapons */
#define O_CONS               26         /* Miscellaneous consumables */
#define O_TRAP               27         /* Usable Traps */
#define O_HAND               28         /* Must be held in a hand to function */
#define O_THRN               29         /* Charged throwable */
#define O_CARD               30         /* Collectable Card */
#define O_GACH               31         /* Gachas */

#define C_FGHT                1
#define C_PALD                2
#define C_RANG                3
#define C_CLRC                4
#define C_DRUD                5
#define C_MAGE                6
#define C_ASSN                7
#define C_HIER                8
#define C_CVLR                9
#define C_MONK               10
#define C_NOBL               11

#define I_RHND                1
#define I_LHND                2
#define I_LSHL                3
#define I_RSHL                4
#define I_NCK1                5
#define I_NCK2                6
#define I_BLTL                7
#define I_BLTR                8
#define I_BLTF                9
#define I_BLTB               10
#define I_SHLD               11
#define I_ARMR               12
#define I_RNG1               13
#define I_RNG2               14
#define I_RNG3               15
#define I_RNG4               16
#define I_ROBE               17
#define I_BOOT               18
#define I_HELM               19

/* Object effects */
#define E_NONE                0
#define E_HEAL                1
#define E_ARMR                2
#define E_ATCK                3
#define E_FEAR                4
#define E_SLEP                5
#define E_TELE                6              /* ???? */
#define E_SPCL                7
#define E_POSN                8
#define E_BLES                9
#define E_PROT               10
#define E_INTR               11
#define E_INHT               12              /* increase to hit */
#define E_DAMG               13              /* cause damage */
#define E_ITEM               14              /* summon an item */
#define E_MANA               15              /* restore mana */
#define E_SOCK               16              /* Item can be placed in a socket */
#define E_ENCH               17              /* Enchant an item */
#define E_MUTA               18              /* Item is mutable */
#define E_LGCY               19              /* Legacy item */
#define E_TEMP               20              /* Temporarity effect, ctl=PE_, dmg=length */
#define E_WGHT               21              /* Allow for increased weight carrying */
#define E_TRAP               22              /* Settable trap */
#define E_RSPL               23              /* Random spell scroll, ct1=min spell level, ct2=max spell level */
#define E_SPEL               24              /* Specific spell scroll, ct1=spell, ct2=spell control */
#define E_LGND               25              /* Mutable legendary */
#define E_SUMN               26              /* Summon */

/* Socket effects */
#define S_STR               100
#define S_DEX               200
#define S_INT               300
#define S_WIS               400
#define S_CON               500
#define S_CHR               600
#define S_MAN               700
#define S_HPS               800
#define S_ATP               900
#define S_DEF              1000
#define S_HIT              1100
#define S_HPR              1200              /* HP regeneration */
#define S_MNR              1300              /* Mana regeneration */
#define S_ALL              1400              /* All base stats */
#define S_EXP              1500              /* Increase Experience Gained */
#define S_MGF              1600              /* Increase Magic Find */
#define S_MAG              1700              /* Increase Hp/Mana */
#define S_MGR              1800              /* Resistance to magic damage */
#define S_MLV              1900              /* Increase Monster Level */
#define S_CRT              2000              /* Increase critical chance */
#define S_DDG              2100              /* Increase dodge */
#define S_FIR              2200              /* Elemental fire */
#define S_CLD              2300              /* Elemental cold */
#define S_ELC              2400              /* Elemental electrical */
#define S_ARC              2500              /* Elemental arcane */
#define S_SPP              2600              /* Increase spell power */
#define S_ELE              2700              /* Resist any elemental attack, Same as A_RELE */
#define S_RPS              2800              /* Resist poison */
#define S_IPS              2900              /* Immune poison */
#define S_FCE              3000              /* S_FIR+S_CLD+S_ELC+S_ARC */
#define S_BOW              3100
#define S_1HSWORD          3200
#define S_SPEAR            3300
#define S_MACE             3400
#define S_SCOUT            3500
#define S_LANG             3600
#define S_LORE             3700
#define S_MAGIC            3800
#define S_HEAL             3900
#define S_MED              4000
#define S_RIDE             4100
#define S_BOAT             4200
#define S_ORACLE           4300
#define S_CHARM            4400
#define S_SURVIVE          4500
#define S_COMBAT           4600
#define S_DAGGER           4700
#define S_2HSWORD          4800
#define S_AXE              4900
#define S_FLAIL            5000
#define S_CROSSBOW         5100
#define S_TRIDENT          5200
#define S_POLEAXE          5300
#define S_THROWN           5400

#define S_MAX_ENCH         31                /* Highest socket effect for applying random enchants */

/* Intrinsic effects */
#define A_NOSL                1              /* No sleep */
#define A_NOPS                2              /* No poison */
#define A_RSFR                3              /* Resist Fear */
#define A_LEV                 4              /* Levitation */
#define A_SPLV                5              /* Speed and Levitation */
#define A_SRCH                6              /* Auto Search */
#define A_SPD                 7              /* speed */
#define A_HEAL                8              /* Faster hp recovery */
#define A_REFL                9              /* Reflect energy */
#define A_RFIR               10              /* Resist Fire */
#define A_RELE               11              /* Resist any elemental attack */
#define A_XRAY               12              /* xray vision */
#define A_GSEE               13              /* See inside gateway */
#define A_MANA               14              /* Reduces mana cost */
#define A_NOFR               15              /* No Fear */
#define A_TRAP               16              /* Autodetect Traps */
#define A_IDNT               17              /* Auto Identification */
#define A_CURS               18              /* Detect Curse */
#define A_NPHL               19              /* Increased hp recovery while napping */
#define A_DTFD               20              /* Autodetect food */
#define A_DTUP               21              /* Autodetect up stairs */
#define A_DTDN               22              /* Autodetect down stairs */
#define A_DEXP               23              /* Double Experience Gain */
#define A_IEXP               24              /* Increased Experience Gain */
#define A_RDLR               25              /* Read Lore */
#define A_RDLN               26              /* Read Language */
#define A_RALL               27              /* Read All */
#define A_GACT               28              /* Activate gateway */
#define A_GATE               29              /* Allow gateway to be seen */
#define A_TARD               30              /* Tardis */
#define A_COMP               31              /* Compass */
#define A_FOOD               32              /* Removes need for food */
#define A_BOAT               33              /* Travel on water */
#define A_FLY                34              /* Upland flight */
#define A_FCNG               35              /* Negate force field damage */
#define A_RCLD               36              /* Resist cold */
#define A_RELC               37              /* Resist Electric */
#define A_RDRK               38              /* Resist Dark */
#define A_RPSI               39              /* Resist Psionic */
#define A_RSSL               40              /* Resist sleep */
#define A_RSPS               41              /* Resist poison */
#define A_WATER              42              /* walk on water */
#define A_NDRN               43              /* Prevents drain level effect */
#define A_INFRA              44              /* Infravision */

#define SHOP_ALCH             1              /* Alchemist Shop */
#define SHOP_ARMR             2              /* Armour Shop */
#define SHOP_BOOK             3              /* Book Store */
#define SHOP_ROBE             4              /* Clothing Shop */
#define SHOP_JEWL             5              /* Jeweler */
#define SHOP_MAGC             6              /* Magic shops */
#define SHOP_WEAP             7              /* Weapons */
#define SHOP_FOOD             8              /* Provisions */
#define SHOP_CRFT            17              /* Crafting */
#define SHOP_SPEL            18              /* Spells */
#define SHOP_RECP            19              /* Recipes */

#define ITEM_SRATIONS         10000          /* link code for standard rations */

/* Player temporary effects */
#define PE_POISON             0              /* time player is poisoned */
#define PE_PROT               1              /* Time that player is protected */
#define PE_APROT              2
#define PE_FIRE               3              /* Time player is protected from fire */
#define PE_HASTE              4              /* Time player is hasted */
#define PE_GIANT              5              /* Time player has giant strength */
#define PE_ETHER              6              /* Time player is ethereal */
#define PE_FEAR               7              /* Time player is feared */
#define PE_SLEEP              8              /* Time player is slept */
#define PE_LIGHT              9              /* Time player has light */
#define PE_BLIND             10              /* Time player is blinded */
#define PE_CONFUSED          11              /* Time player is confused */
#define PE_READLANG          12              /* Time player can read languagees */
#define PE_READLORE          13              /* Time player can read lore */
#define PE_LEV               14              /* Time player is levitated */
#define PE_COLD              15              /* Time player has cold resistance */
#define PE_ELEC              16              /* Time player has electric resistance */
#define PE_ARCN              17              /* Time player has arcane resistance */
#define PE_ROOT              18              /* Time player is rooted */
#define PE_MIND              19              /* Time player is shielded from scionic attacks */
#define PE_DOT               20              /* Damage over time */
#define PE_HEROIC            21              /* Heroic mode */
#define PE_DEPTH             22              /* Dungeon depth */
#define PE_REBIRTH           23              /* Holds rebirth count */
#define PE_POLYMORPH         24              /* Monster polymorphed into */
#define PE_POLYTIME          25              /* Time remaining in polymorph */
#define PE_INVIS             26              /* Player cannot be seen */
#define PE_HCOUNT            29              /* Time til next healing */
#define PE_COMPASS           30              /* Compass Target */
#define PE_NOARENA           31              /* When nonzero will not make multi-monster groups */
#define PE_MCOUNT            32              /* Number of monsters that will spawn on level */
#define PE_DUNGEON           33              /* Dungeon number, used by sun stone */
#define PE_DLEVEL            34              /* Dungeon level, used by sun stone */
#define PE_WORLDSEED         35              /* World seed */

#define STAT_MOBS_KILLED       0              /* Core */
#define STAT_GOLD_LOOTED       1              /* Core */
#define STAT_STEPS_TAKEN       2              /* Core */
#define STAT_HOURS_NAPPED      3              /* Core */
#define STAT_ALCOHOL_DRANK     4              /* Core */
/*
#define STAT_SEX_CHANGE        5
*/
#define STAT_COMMONS           6              /* Core */
#define STAT_UNCOMMONS         7              /* Core */
#define STAT_RARES             8              /* Core */
#define STAT_VERY_RARES        9              /* Core */
#define STAT_ULTRA_RARES      10              /* Core */
#define STAT_LEVELS_CLEARED   11              /* Core */
#define STAT_DUNGEON_CLEARED  12              /* Core */
#define STAT_ITEMS_CRAFTED    14              /* Core */
#define STAT_SOCKETS_ADDED    15              /* Core */
#define STAT_ENCHANTMENTS     16              /* Core */
#define STAT_DRAGONS          17              /* Core */
#define STAT_SCROLLS_READ     18              /* Core */
#define STAT_POTIONS_DRUNK    19              /* Core */
#define STAT_MEALS_EATEN      20              /* Core */
/*
#define STAT_ROCK_HOUND       21
*/
#define STAT_ARROWS_FIRED     22              /* Core */
/*
#define STAT_EGGS_FOUND       23
#define STAT_MEAT_EATEN       24
#define STAT_POTATOS_EATEN    25
#define STAT_APPLEUP_EATEN    26
*/
#define STAT_POISONED         27              /* Core */
#define STAT_DAMAGE           28              /* Core */
#define STAT_HARDEST_HIT      29              /* Core */
#define STAT_DAMAGE_TAKEN     30              /* Core */
#define STAT_HARDEST_TAKEN    31              /* Core */
#define STAT_ITEMS_SOLD       32              /* Core */
#define STAT_SOLD_GOLD        33              /* Core */
#define STAT_LARGEST_SALE     34              /* Core */
#define STAT_INTEREST         35              /* Core */
#define STAT_CONVERSATIONS    36              /* Core */
#define STAT_ITEMS_IDENTED    37              /* Core */
#define STAT_CLASSES_TAKEN    38              /* Core */
#define STAT_HARD_MOB         39              /* Core */
/*
#define STAT_BSTONE_RINGS     40
#define STAT_GREEN_BEERS      41
#define STAT_CORNED_BEEF      42
#define STAT_BRIDE            43
#define STAT_GROOM            44
#define STAT_CHARMS           45
#define STAT_FIREWORKS        46
#define STAT_CANDY            47
#define STAT_COSTUMES         48
#define STAT_TURKEY_DINNERS   49
#define STAT_PRESENTS_OPENED  50
#define STAT_SANTA            51
#define STAT_MISTLETOE        52
#define STAT_CANDY_HEARTS     53
#define STAT_CLOVERS          57
*/
#define STAT_HITS             54
#define STAT_MISSES           55
#define STAT_CRITS            56
/*
#define STAT_FIGURINES_USED   58
#define STAT_BANDAGES_USED    59
#define STAT_SUN_SCROLLS_USED 60
#define STAT_SNAKE_SCROLLS    61
#define STAT_FOOD_SCROLLS     62
#define STAT_MONSTER_CARDS    63
#define STAT_GACHA            64
#define STAT_CLASS_CARDS      65
#define STAT_WEAPON_CARDS     66
#define STAT_ARMOR_CARDS      67
#define STAT_ITEM_CARDS       68
#define STAT_USE_DECK         69
#define STAT_GET_DECK         70
#define STAT_EQUIP_MYTHIC     71
#define STAT_SUNSTONE         72
#define STAT_PORTABLE_HOLE    73
#define STAT_MECH_MERCHANT    74
*/

#define M_NONE                 0
#define M_MAGIC                1
#define M_BOW                  2
#define M_PHYSICAL             3

#define CLR_BLACK              0
#define CLR_BLUE               1
#define CLR_GREEN              2
#define CLR_CYAN               3
#define CLR_RED                4
#define CLR_PURPLE             5
#define CLR_BROWN              6
#define CLR_GRAY               7
#define CLR_DARKGRAY           8
#define CLR_LIGHTBLUE          9
#define CLR_LIGHTGREEN        10
#define CLR_LIGHTCYAN         11
#define CLR_LIGHTRED          12
#define CLR_LIGHTPURPLE       13
#define CLR_YELLOW            14
#define CLR_WHITE             15

typedef char OUTSTRING[32];
typedef char STRING32[32];
typedef char STRING13[13];
typedef char STRING128[128];
typedef char MSTRING[128];
typedef int  MAP_TYPE[101][101];
typedef byte KNOW_TYPE[101][101];
typedef int  SPELLBOOK[1000];

typedef struct {
  Int32  item;
  Int32  percent;
  } HUNTING_ITEMS;

typedef struct {
  Int32  class;
  Int32  level;
  Int32  trait;
  } TRAITS;

typedef struct {
  char* name;
  Int32 stat_adjust[6];
  Int32  fixed_skill[4];
  Int32  fixed_skill_range[4];
  Int32  flags;
  Int32  aging;
  } RACE_REC;

typedef struct {
  char*  name;
  Int32  stat_adjust[6];
  double first_exp;
  double next_exp;
  Int32  initial_inv_slot[4];
  Int32  initial_inv_item[4];
  Int32  hitdie;
  Int32  mana;
  Int32  becomes_caster;
  Int32  random_skill[3];
  Int32  random_skill_range;
  Int32  fixed_skill[4];
  Int32  fixed_skill_range[4];
  Int32  flags;
  Int32  skill_table;
  Int32  level_skills[4];
  Int32  spell_group;
  Int32  hp_regen;
  Int32  weapon_affinity;
  Int32  sprite;               /* drawn for the player with the Unique Class Sprites preference */
  } CLASS_REC;

typedef struct {
  Int32  from;
  Int32  level;
  Int32  to;
  } CLASS_CHANGE_REC;

typedef struct {
  Int32  class;
  Int32  level;
  char*  male;
  char*  female;
  } TITLE_REC;

typedef struct STROKE_FONT STROKE_FONT;

typedef struct {
  Int32 counts[255];
  Int32 trophies[255];
  Int32 num_trophies;
  } STATS_REC;

typedef struct {
  char version[3];
  } SCORE_VER;

typedef struct {
  char version[3];
  char legend_name[12+1][33];
  char legend_title[12+1][33];
  double legend_exp[12+1];
  char legend_mode[12+1];
  char high_name[12+1][33];
  char high_title[12+1][33];
  double high_exp[12+1];
  char high_mode[12+1];
  } SCORE_REC_600;

typedef struct {
  char name[33];
  char title[33];
  double exp;
  char mode;
  } SCORE_RECORD;

typedef struct {
  char   name[33];
  double exp;
  Int32  level;
  char   cause;
  byte   class;
  Int32  time_played;
  char   mode;
  char   permadeath;
  } SCORE_ENTRY;

typedef struct {
  char*  name;
  char*  title;
  double exp;
  } DEFAULT_SCORE_REC;

typedef struct {
  Int32 link;
  byte  region;
  char  message[256];
  } MESSAGE;

typedef struct {
  Int32  shoptype;
  Int32  region;
  Int32  rx;
  Int32  ry;
  Int32  cx;
  Int32  cy;
  char name[40];
  } SHOP_REC;

typedef struct {
  Int32 region;
  Int32 x;
  Int32 y;
  Int32 level;
  Int32 map;
  } PREDEF_MAP;

typedef struct {
  Int32 quest;
  Int32 message;
  } COMPASS;

typedef struct {
  Int32 map;
  Int32 x;
  Int32 y;
  } TR_SHIP_REC;

typedef struct {
  Int32  quest;
  Int32  region;
  Int32  x;
  Int32  y;
  Int32  typ;
  Int32  diff;
  char name[33];
  Int32  treasure;
  Int32  premium;
  Int32  sx;
  Int32  sy;
  Int32  boss;
  Int32  adds;
  Int32  numadds;
  } LINK_REC;

typedef struct {
  char  name[33];
  } REGION_REC;

typedef struct {
  Int32    speed;
  Int32    number;
  Int32    hp;
  Int32    max_hp;
  Int32    mana;
  Int32    level;
  Int32    attack;
  Int32    defense;
  double exp;
  double exp_hp;
  UInt32    special;
  UInt32    special2;
  Int32    sprite;
  Int32    mtype;
  char   name[20];            /* 16 left "Chromatic Dragon" unterminated, which corrupted saves */
  Int32    time;
  Int32    sleep;         /* -sleep is rooted, +sleep is sleeping */
  Int32    fear;
  Int32    blind;
  Int32    confused;
  Int32    poison;
  Int32    prev;
  Int32    treasure;
  } NPC_REC;

typedef struct {       /* this is used for setting character stats with transformations */
  Int32    number;
  Int32    strength;
  Int32    dexterity;
  Int32    intelligence;
  Int32    wisdom;
  Int32    constitution;
  Int32    charisma;
  } NPC_STATS;

typedef struct {
  Int32  link;
  Int32  otype;
  Int32  user_hp;
  Int32  user_health;
  byte effect;
  Int32  control;
  Int32  control2;
  byte weight;
  double cost;
  Int32    level;
  Int32    damage;
  Int32    sprite;
  Int32    rarity;
  Int32    plus;
  Int32    charges;
  double costp;
  Int32    align;
  Int32    skill;                  /* controlling skill */
  Int32    min_skill;              /* minimum skill to use */
  char name[60];
  Int32    num_enchantments;
  Int32    enchantments[3];
  Int32    num_sockets;
  Int32    sockets[3];
  UInt32   flags;
  Int32    flags2;
  } OBJECT_REC;

typedef struct {
  char  main_hand[60];
  char  off_hand[60];
  char  neck_1[60];
  char  neck_2[60];
  char  shield[60];
  char  armor[60];
  char  ring_l1[60];
  char  ring_l2[60];
  char  ring_r1[60];
  char  ring_r2[60];
  char  cloak[60];
  char  boots[60];
  char  helm[60];
  } GEAR_SET;

typedef GEAR_SET GEAR_MANAGER[10];

typedef struct {
  Int32 object;
  Int32 craft_item;
  Int32 object1;
  Int32 object2;
  Int32 object3;
  Int32 object4;
  Int32 object5;
  Int32 object6;
  Int32 object7;
  Int32 object8;
  Int32 object9;
  Int32 object10;
  } RECIPE;

typedef struct {
  byte user_hp;
  byte user_health;
  Int32  effect;
  Int32  control;
  byte allign;
  Int32 number;
  byte level;
  byte mana;
  UInt32  special;
  byte for_sale;
  char name[26];
  } SPELL_REC;

typedef struct {
  Int32        region;
  Int32        rx;
  Int32        ry;
  Int32        cx;
  Int32        cy;
  double     mortgage;
  double     payment;
  double     interest;
  OBJECT_REC inventory[500];
  } HOUSE;

typedef struct {
  Int32        region;
  Int32        rx;
  Int32        ry;
  Int32        cx;
  Int32        cy;
  double     price;
  } FORSALE;

typedef struct {
  Int32        citynum;
  Int32        region;
  Int32        rx;
  Int32        ry;
  } CITY_NUMBER;

typedef NPC_REC DUNGEON_MONSTERS[6][61];
typedef OBJECT_REC DUNGEON_OBJECTS[1000];

typedef struct {
  MAP_TYPE         map;
  KNOW_TYPE        known;
  DUNGEON_MONSTERS monsters;
  DUNGEON_OBJECTS  objects;
  Int32              upx;
  Int32              upy;
  Int32              downx;
  Int32              downy;
  Int32              cleared;
  } DUNGEON_LEVEL;

typedef struct {
  Int32          num_levels;
  Int32          dungeon_number;
  DUNGEON_LEVEL level[10];
  Int32          treasurex;
  Int32          treasurey;
  } DUNGEON;

/* What the Load Game screen shows of a save, read by load_summary(). */
typedef struct {
  Int32       class;
  Int32       level;
  Int32       mode;
  Int32       time_played;
  } SAVE_SUMMARY;

/* A saved game in the working directory, and the list of them that savelist.c reads, newest first */
typedef struct {
  char         name[80];
  time_t       saved;
  SAVE_SUMMARY summary;
  Int32        readable;
  } SAVED_GAME;

typedef struct {
  SAVED_GAME* games;
  Int32       count;
  Int32       capacity;
  } SAVE_LIST;

/* A list's rows under a heading line: count rows, of which shown fit from top. A list longer than that scrolls, with
   "more" markers at more_column and a scroll bar after them. Its columns and lines are the 80x25 text grid's, or with
   a row_height a panel's: 8-pixel columns from x and row_height-pixel lines from y. */
typedef struct {
  Int32       count;
  Int32       shown;
  Int32       top;
  Int32       line;
  Int32       more_column;
  Int32       x;
  Int32       y;
  Int32       row_height;
  } SCROLL_LIST;

/* A list picked from with a cursor, as on the Game Options screen. draw_row draws a row on its line, reaching the
   screen's own data through screen. */
#define MENU_KEYS "(8/2) Select   (ENTER) Choose   (ESC) Leave"

#define TAB_LOAD_GAME   -1    /* open_tab_menu(): a game to load was picked, named in loadname */

typedef struct MENU MENU;

struct MENU {
  SCROLL_LIST rows;
  Int32       row;
  Int32       column;
  Int32       value_column;
  Int32       color;
  char*       keys;
  Int32       keys_line;
  void        (*draw_row)(MENU* menu,Int32 row,Int32 line);
  void*       screen;
  };

/* Stored items listed with the cursor (storage.c): the slots from the first listed, the item the screen holds in the
   air, and the digits of the slots' numbers */
typedef struct {
  OBJECT_REC* slots;
  OBJECT_REC* inair;
  Int32       digits;
  } STORAGE;

#define STORAGE_MOVED   'M'   /* storage_key(), storage_action(): items were moved or sorted, so draw them again */

typedef struct {
  char      name[21];
  byte      age;
  char      sex;
  word      level;
  double    exp;
  double    exp_next;
  byte      class;
  char      title[26];
  byte      race;
  Int32       stat[7];
  Int32       mhp;
  Int32       chp;
  byte      health;
  Int32       mmana;
  Int32       cmana;
  Int32       ability[11];
  Int32       x;
  Int32       y;
  Int32       map_x;
  Int32       map_y;
  MAP_TYPE  map;
  KNOW_TYPE known;
  Int32       region;
  Int32       skill[31];
  Int32       torch;
  Int32       food;
  Int32       armour;
  Int32       weapon;
  Int32       mount;
  Int32       peg;
  Int32      gold_coins;
  Int32      mithril_coins;
  Int32      adamantite_coins;
  Int32      bank;
  Int32      interest;
  byte      d_level;
  byte      diff;
  byte      land_x;
  byte      land_y;
  char      d_name[31];
  byte      day;
  byte      month;
  byte      min;
  byte      hour;
  OBJECT_REC invent[20];
  OBJECT_REC pack[33];
  Int32       effect[255];
  DUNGEON_MONSTERS monster;
/*  NPC_REC   monster[6][61]; */
  NPC_REC   party[6];
  Int32       align;
  char      in_city;
  byte      city;
  Int32       hp_count;
  Int32       mana_count;
  Int32       phase;
  byte      gen[5000];
  byte      lore[5000];
  OBJECT_REC pawn[10][11];
  STRING32  fspell[13];
  Int32       ship_x;
  Int32       ship_y;
  Int32       ship_r;
  char      inship;
  char      gateway;
  byte      tardis_x[11];
  byte      tardis_y[11];
  byte      tardis_r[11];
  STRING32  tardis_d[11];
  char      cav_paid;
  DUNGEON_OBJECTS map_objects;
/*  OBJECT_REC map_objects[1000]; */
  OBJECT_REC hole[1000];
  HOUSE      houses[5];
  Int32        dungeon_number;
  char       quests[1000];
  Int32       time_played;
  } PLAYER_REC;

LINK PLAYER_REC         player;

#include "spells.h"
#include "world.h"
#include "lore.h"
#include "gen.h"
#include "objects.h"
#include "monsters.h"
#include "recipe.h"
#include "races.h"
#include "classes.h"
#include "trophies.h"


/* LINK char**             sprite[255]; */
LINK DUNGEON            current_dungeon;
LINK GEAR_MANAGER       gear_manager;
LINK SPELLBOOK          spellbook;
LINK Int32                a_mon;
LINK char               soundon;
LINK Int32                bottom;
LINK Int32                word_counter;
LINK char               clair;
LINK Int32                snakes;
LINK Int32                inv_item;
LINK char               napping;
LINK Int32                exitf;
LINK byte               dc;
LINK char               combat_mode;
LINK Int32                mod_exit;
LINK byte               score1;
LINK byte               score2;
LINK char              exit_flag;
LINK char               restock;
LINK char               loadname[80];
LINK char               backupname[80];
LINK char               graphics;
LINK char               messages[MAX_MSG][80];
LINK Int32                msg_clr[MAX_MSG];
LINK Int32                msg_name_clr[MAX_MSG];   /* an item name's rarity colour from msg_name_at[] on; 0 for none */
LINK Int32                msg_name_at[MAX_MSG];
LINK STATS_REC          stats;
LINK Int32                recipe_start;
LINK Int32                heroicMode;
LINK char               in_arena;
LINK byte               pixel;
LINK OBJECT_REC         shop_items[1000];
LINK Int32                n_shop_items;
LINK char               wizardmode;
LINK Int32                NO_OF_OBJECTS;
LINK Int32                TOTAL_OBJECTS;
LINK Int32                NO_OF_GENS;
LINK Int32                NO_OF_LORE;
LINK Int32                recipe_count;
LINK Int32                num_spells;
LINK SCORE_ENTRY        hall_of_fame[MAX_HALL_OF_FAME];
LINK SCORE_ENTRY        final_scores[MAX_FINAL_SCORES];
LINK SCORE_REC_600      scores_600;
LINK SCORE_VER          score_version;
LINK SCORE_RECORD       high_scores[NUM_CLASSES];
LINK SCORE_RECORD       legend_scores[NUM_CLASSES];
LINK Int32                num_hall_of_fame;
LINK Int32                num_final_scores;
LINK char               debugMSG[256];
LINK Int32                hall_position;
LINK Int32                death_position;
LINK char               music;
LINK Int32                music_volume;
LINK char               game_thread;
LINK char               integer_scaling;
LINK Int32                target_x;
LINK Int32                target_y;
LINK double             dir_x;
LINK double             dir_y;
LINK Int32                currentFloor;
LINK Int32                lastFloor;
LINK Int32                offset_x,offset_y;
LINK Int32                trophy_count;
LINK Int32               start_time;
LINK Int32               end_time;
LINK struct tm          date;
LINK Int32                score_position;
LINK Int32                path_x[PATH_MAX];
LINK Int32                path_y[PATH_MAX];
LINK Int32                best_path;
LINK Int32                dungeon_floor;
LINK Int32                dungeon_wall;
LINK OBJECT_REC         in_air;
LINK char               buffer[64];
LINK char               debugbuffer[255];
LINK char               skipcheck;
LINK char               in_upland;
LINK char               show_keys;
LINK OBJECT_REC*        item_to_use;
LINK char               auto_pickup_gold;
LINK char               auto_pickup_torch;
LINK char               auto_open_doors;
LINK char               auto_bump_walls;
LINK Int32                drop_x;
LINK Int32                drop_y;
LINK char               use_from_ground;
LINK char               no_arena;
LINK Int32                downgrade_depth;
LINK char               downgrade_arena;
LINK Int32                run_x;
LINK Int32                run_y;
LINK char               dungeon_scaling;
LINK Int32                monster_count;
LINK char               party_combat_mode;
LINK Int32                delay_ranged;
LINK Int32                delay_hit;
LINK Int32                delay_trap;
LINK char               enhanced_sprites;
LINK char               floor_item_names;
LINK char               gothic_scrolls;
LINK char               improved_interface;
LINK char               show_mount;
LINK char               class_sprites;
LINK char               tab_menu;
LINK char               permadeath;
LINK char               change_eshter_spawns;
LINK char               change_dalek_spawns;
LINK char               change_weapon_damage;
LINK char               change_scroll_charging;
LINK char               change_negative_amounts;
LINK char               change_teleport_spell;
LINK char               change_blink_only;
LINK char               change_blink_landing;
LINK char               change_sleep_traps;
LINK char               change_tardis_saves;
LINK char               change_tardis_descriptions;
LINK char               change_item_deck;
LINK char               change_tripped_card;
LINK char               change_lore_search;
LINK char               change_library_text;
LINK char               change_arena_assassins;
LINK char               change_party_joining;
LINK char               change_spell_targets;
LINK char               change_blast_spells;
LINK char               change_spell_kills;
LINK char               change_thrown_weapons;
LINK char               change_poison_kills;
LINK char               change_temple_uncurse;
LINK char               change_ship_purchase;
LINK char               change_pack_weight;
LINK char               change_possessions;
LINK char               change_bodily_health;
LINK char               change_compass;
LINK char               change_gateway_exit;
LINK char               change_new_game_dungeon;
LINK char               change_words_of_power;
LINK char               change_sun_stone;
LINK char               change_sun_stone_gateway;
LINK char               change_upland_quit;
LINK char               change_ranged_damage;
LINK char               change_summon_combat;
LINK char               change_house_storage;
LINK char               change_house_buying;
LINK char               change_shop_boots_wands;
LINK char               change_arena_exit;
LINK char               change_arena_decorations;
LINK char               change_arena_monk_fear;
LINK char               change_mind_shield;
LINK char               change_meat_potatoes;
LINK char               change_world_seed;
LINK char               change_arena_timers;
LINK char               change_last_bite;
LINK char               change_floor_items;
LINK char               change_quest_state;
LINK char               change_valley_of_fear;
LINK char               change_sun_stone_fixes;
LINK char               change_lore_books;
LINK char               change_hunting_skill;
LINK char               change_free_upland_keys;
LINK char               change_shaman_legendaries;
LINK char               change_platinum_dragon_mount;
LINK char               change_town_completion;
LINK char               change_town_trophy;
LINK char               change_village_shops;
LINK char               change_nether_core;
LINK char               change_ship_improvements;
LINK Int32                last_target_x;
LINK Int32                last_target_y;
LINK char               used_sunstone;
LINK Int32                col;
LINK Int32                row;
LINK char               auto_target;
LINK Int32                levelCleared;
LINK Int32                screen_width;
LINK Int32                screen_height;
LINK Int32                TEXT_GRID_WIDTH;
LINK Int32                TEXT_GRID_HEIGHT;
LINK Int32                smap_offset_x;
LINK Int32                smap_offset_y;
LINK Int32                tile_width;
LINK Int32                tile_height;
LINK Int32                TILES_X;
LINK Int32                TILES_Y;
LINK Int32                NUM_MSG;
LINK Int32                text_offset_x;
LINK Int32                text_offset_y;
LINK Int32                showStats;
LINK char               fkey_mode;
LINK char               macros[12][1024];
LINK char               currentMacro[1024];
LINK char               takes_time;
LINK KNOW_TYPE          known_maps[51];
LINK MAP_TYPE           maps[MAP_COUNT];
LINK char               encrypt_save_file;


#ifdef DEBUG
LINK Int32                debug_tab;
#endif


/* advance.c */
extern void advancement();
extern void advance_stat();

/* cave.c */
extern void cave();

/* combat.c */
extern void arena(Int32 monster,Int32 floor,Int32 barriers,Int32 srcx,Int32 srcy);

/* help.c */
extern void combat_help();
extern void dungeon_help();
extern void show_icons();
extern void upland_help();

/* mapmaint.c */
extern void arena_move_enemy(Int32 x,Int32 y);
extern void arena_move_party(Int32 x,Int32 y);
extern void make_map(char dir);
extern void move_enemy(Int32 x,Int32 y);
extern void arena_move_snake(Int32 x,Int32 y,Int32 floor);
extern void move_snake(Int32 x,Int32 y,Int32 floor);
extern void stock_monsters(Int32 depth,Int32 treasurex,Int32 treasurey,MAP_TYPE* map,DUNGEON_MONSTERS* monsters);
extern void stock_town(byte town);
extern void stock_treasure();

/* shops.c */
extern void dragon_dice();
extern void gamble();
extern Int32 haggle(double price,OBJECT_REC item);
extern void cavalier(char* name);
extern void library(char* name);
extern void mounts(char* name);
extern void non_magic_weapons(char* name);
extern void pawn(char* name, byte city);
extern void provisions(char* name);
extern void school(char* name);
extern void tavern(char* name);
extern void temple(char* name);
extern void travel(char* name);
extern void bank(char* name);

/* specials.c */
extern void perform_effect(OBJECT_REC* obj);
extern void special(Int32 whichone,OBJECT_REC* item);
extern void set_gateway_dungeon();
extern Int32 check_quit(void);
extern void eat(void);
extern Int32 party_blow(NPC_REC* attacker);
extern Int32 trophy_counter(Int32 trophy,Int32 slot);
extern void tick_polymorph(void);
extern char is_valley_of_fear(Int32 i);
extern Int32 link_y(Int32 i);
extern void show_floor_item(void);
extern Int32 ship_region(void);
extern void track_ship(void);
extern void place_town_ship(void);
extern void ship_leaves_town(void);
extern void dock_square(Int32* x,Int32* y);
extern Int32 find_harbour(Int32 city,Int32* x,Int32* y);
extern Int32 sails_into_town(Int32 x,Int32 y);
extern void ship_docks_in_town(void);
extern void walk_aboard(void);
extern Int32 steps_ashore(Int32 x,Int32 y,Int32 dx,Int32 dy,Int32 shore);
extern void show_bottom_name(char* name,Int32 color);
extern void hide_bottom_name(void);
extern Int32 list_shows(SCROLL_LIST* list,Int32 row);
extern Int32 list_scroll_to(SCROLL_LIST* list,Int32 row);
extern Int32 list_line(SCROLL_LIST* list,Int32 row);
extern void list_text(SCROLL_LIST* list,Int32 column,Int32 line,Int32 color,char* text);
extern void draw_list_more(SCROLL_LIST* list);
extern Int32 list_arrow_width(SCROLL_LIST* list);
extern void draw_list_arrow(SCROLL_LIST* list,Int32 column,Int32 line,Int32 selected);
extern void fit_menu(MENU* menu,Int32 count);
extern void write_centred(Int32 line,Int32 color,char* text);
extern void draw_menu_keys(char* keys,Int32 line);
extern void hide_menu_keys(MENU* menu);
extern void show_menu_question(MENU* menu,Int32 color,char* question);
extern void show_item_question(MENU* menu,Int32 color,char* before,OBJECT_REC* item,char* name,char* after);
extern void draw_menu(MENU* menu);
extern Int32 menu_label_width(MENU* menu);
extern void draw_menu_text(MENU* menu,Int32 line,char* label,char* value);
extern void draw_menu_text_in(MENU* menu,Int32 line,char* label,char* value,Int32 color);
extern void draw_menu_named(MENU* menu,Int32 line,char* name,Int32 name_color,char* value);
extern void draw_menu_choice(MENU* menu,Int32 row,Int32 line);
extern char menu_key(MENU* menu);
extern Int32 menu_choice(MENU* menu);
extern char menu_number(MENU* menu);
extern Int32 ask_choice(Int32 column,Int32 line,Int32 color,char** choices,Int32 count,Int32 row);
extern void open_storage(MENU* menu,STORAGE* storage,Int32 count);
extern char storage_key(MENU* menu);
extern char storage_action(MENU* menu,char* main);
extern char* transfer_action(OBJECT_REC* item);
extern void show_in_air(OBJECT_REC* in_air,Int32 line);
extern char ask_action(Int32 column,Int32 line,Int32 color,char* action,char* cancel,Int32 row);
extern Int32 open_tab_menu(void);
extern Int32 merchant_beside(void);
extern void clear_messages(void);
extern void special8();
extern void special9();
extern void special14();
extern void special22();
extern void special31(Int32 max);
extern void special42(Int32 mon);
extern void special43();
extern void special53();
extern void special55();
extern void special61(OBJECT_REC* item);
extern void special62(OBJECT_REC* item);
extern void special63(OBJECT_REC* item);
extern void special68();
extern void special69();
extern void special70();
extern void special71();
extern void special77();
extern void special80();
extern void special81();

/* terminal.c */
extern void bar(Int32 x1,Int32 y1,Int32 x2,Int32 y2);
extern void clrscr();
extern void delay(Int32 time);
extern void draw(Int32 x1,Int32 y1,Int32 x2,Int32 y2,Int32 clr);
extern void gotoxy(Int32 x,Int32 y);
extern void graphmode();
extern void init_graph(GAME_HOST* host);
extern Int32 keypressed();
extern void outtextxy(Int32 x,Int32 y,char* buffer);
extern void outtextxyclr(Int32 x,Int32 y,char* buffer);
extern void outtext_xy(Int32 x,Int32 y,char* buffer);
extern void putimage(Int32 x,Int32 y,char** img,Int32 typ);
extern Int32 readkey();
extern void readln(char* buffer);
extern Int32 p_read(char* buffer);
extern void restoreScreen();
extern void saveScreen();
extern void screenshot();
extern void setcolor(Int32 clr);
extern void setfillstyle(Int32 st,Int32 clr);
extern void settextstyle(Int32 a,Int32 b,Int32 c);
extern void textmode(Int32 mode);
extern void textcolor(Int32 color);
extern void txt_write(char* buffer);
extern void txt_writeln(char* buffer);
extern void close_display();
extern void plot(Int32 x,Int32 y,Int32 clr);
extern void flush();

/* music.c */
extern void play_music(Int32 song);
extern void stop_music();
extern void toggle_music();
extern void set_music_volume();

/* uchar.c */
extern char* fix_name(STRING32 name);
extern char* fix_ability(Int32 ability);
extern char* get_sex();
extern char* get_race();
extern char* get_class();
extern char* get_health();
extern char* get_align();
extern char* yes_no(Int32 input);
extern char* show_health();
extern char* show_align();
extern char* show_int(Int32 n);
extern char* show_flt(double n);
extern void disp_char();
extern void make_char(Int32 rebirth);
extern void print_rec();

/* uinvent.c */
extern char* get_name(OBJECT_REC* item);
extern OBJECT_REC* inventory(OBJECT_REC* item);
extern char inv_valid(OBJECT_REC* item,Int32 line);
extern void show_inventory(OBJECT_REC* item);
extern void compress();
extern void show_lore(Int32 item);
extern OBJECT_REC* unpack(Int32 slot);
extern void do_exchange(OBJECT_REC* item,Int32 line);
extern void pack_item(OBJECT_REC* item,Int32 line);


/* upland.c */
extern void up_land();
extern void stock_encounter();
extern void check_encounter();
extern void take_time();

/* usprites.c */
extern void show_sprite(Int32 x,Int32 y,Int32 sprt);
extern void read_sprites();
extern char** select_sprite(Int32 n);

/* uspells.c */
extern byte has_spell(Int32 check);
extern Int32 check_spell_1(char* spell_name,Int32 level,Int32 allign,Int32 first);
extern Int32 get_spell_name(Int32 column,Int32 search,Int32 level,Int32 allign);
extern Int32 castspell(Int32 spell_num);
extern void new_spell();

/* unisubs.c */
extern void upper(char* buffer);
extern void check_monk_weapon();
extern void check_noble();
extern void check_scores();
extern void copyMap(MAP_TYPE* dest,Int32 num);
extern Int32 read_map(char* filename,MAP_TYPE* map,char* error,Int32 error_size);
extern Int32 load_maps(MAP_TYPE* world,char* error,Int32 error_size);
extern void decode(char* buffer);
extern void winner();
extern Int32 exist(char* filename);
extern Int32 find_gen(char* search);
extern Int32 find_lore(char* search);
extern Int32 found_row(Int32 found);
extern char inkey();
extern void load();
extern Int32 load_summary(char* name, SAVE_SUMMARY* summary);
extern void make_c();
extern void make_h();
extern void make_n();
extern char* make_str(Int32 value);
extern Int32 mana_cost(Int32 mana);
extern Int32 mval(STRING13 instr);
extern double needed(Int32 x);
extern void new_pawn();
extern byte reverseByte(byte in);
extern void show_number();
extern void save_scores();
extern void load_scores();
extern void show_scores();
extern void show_final_scores();
extern void final_score(char cause);
extern Int32 sqr(Int32 n);
extern void teleport();
extern void teleport_spell();
extern void center_view();
extern Int32 rcs_trunc(double x);
extern Int32 u_random(Int32 range);
extern void u_randomize();

/* unisubs2.c */
extern void adjust_abilities();
extern void area9(Int32 damage,Int32 elem,Int32 center_x,Int32 center_y);
extern void area25(Int32 damage,Int32 elem,Int32 center_x,Int32 center_y);
extern void area49(Int32 damage,Int32 elem,Int32 center_x,Int32 center_y);
extern void blind();
extern void clear_prompt();
extern void display_abilities();
extern void do_blink();
extern void do_sleep();
extern void drain(Int32 damage,Int32 elem);
extern void fear(Int32 radius);
extern char get_dir(Int32* xdir,Int32* ydir);
extern char in_sight(Int32 x,Int32 y);
extern void missile(Int32 damage,Int32 elem,Int32 radius);
extern void noise(Int32 f,Int32 d);
extern void prompt(char* display);
extern void promptNS(char* display);
extern void r_missile(Int32 damage);
extern Int32 spell_target(Int32 cell);
extern void spell_kill(NPC_REC* target,Int32 x,Int32 y);
extern void remove_land_player();
extern void remove_player();
extern void show_headings();
extern void show_land();
extern void show_land_player();
extern void show_map();
extern void show_player();
extern void show_screen();
extern void show_side();
extern void show_sm_land();
extern void show_sm_map();
extern void show_spot(Int32 x,Int32 y);
extern void title();
extern Int32 title_menu();
extern Int32 load_menu();
extern void open_save_list(SAVE_LIST* list);
extern void close_save_list(SAVE_LIST* list);
extern void touch(Int32 damage,Int32 elem);

/* unisubs3.c */
extern void close_door();
extern void dart_trap();
extern void fire_trap();
extern void discover();
extern void enemy_missile(Int32 ex,Int32 ey);
extern void fire(Int32 damage,OBJECT_REC* weapon);
extern void go_bottom();
extern void go_down();
extern void go_ship();
extern void go_up();
extern void healing();
extern void lock_door();
extern void nap();
extern void open_door();
extern char door_to_open(void);
extern void square_name(Int32 value,char* name,Int32* color);
extern void pack_it(Int32 floor);
extern void party();
extern void poison_trap();
extern void say_word();
extern void show_log();
extern void sleep_trap();
extern void super(Int32 damage,Int32 elem);
extern void teleport_trap();
extern void view(Int32 floor);

/* writefont.c */
extern void write_font(Int32 x,Int32 y,Int32 color,char* line);
extern Int32 write_font_width(char* line);

/* strokefont.c */
extern STROKE_FONT* stroke_font_create(const byte* data,Int32 length);
extern void stroke_font_draw(const STROKE_FONT* font,Int32 x,Int32 y,Int32 size,Int32 color,char* text);
extern Int32 stroke_font_width(const STROKE_FONT* font,Int32 size,char* text);

/* gothicfont.c */
extern STROKE_FONT* gothic_font();

/* scrolltext.c */
extern void scroll_text(char* message);

extern Int32 check_intrinsic(OBJECT_REC obj,Int32 intr);
extern Int32 find_item_slot();
extern Int32 has_intrinsic(Int32 intr);
extern Int32 intrinsic_count(Int32 intr);
extern void get_shop_items(Int32 otype1,Int32 otype2,Int32 otype3,Int32 otype4,Int32 otype5,Int32 otyp6,Int32 rarity);
extern void limit_shop_items(Int32 max_level);
extern char in_village(void);
extern double item_cost(OBJECT_REC* item);
extern void shop(char* name,Int32 shop_type);
extern Int32 place_object(Int32 level);
extern OBJECT_REC* find_linked_item(Int32 lnk);
extern void dead_monster(NPC_REC* mon,char tr,Int32 x,Int32 y);
extern void poison_kill(NPC_REC* mon);
extern void show_messages();
extern void msg(char* message,Int32 color);
extern void msg_named(char* words,char* name,Int32 color,Int32 name_color);
extern void crafting();
extern void wizard();
extern void show_gen(Int32 item);
extern void buy_house(Int32 hnum);
extern void new_month();
extern void auto_detect();
extern void house_inventory(Int32 hnum);
extern void gain_exp(double amt,Int32 who);
extern void show_quest();
extern void new_game();
extern void convert_time(Int32 t,char* buffer);
extern void debugLog(char* msg);
extern void show_stats();

extern void check_trophies();
extern void show_trophies();
extern void quest_progress(Int32 criteria,Int32* done,Int32* total);

extern void recipe_string(Int32 num,char* buffer);
extern Int32 recipe_ingredient(Int32 num,Int32 i);

/* charattributes.c */
extern Int32 get_base_str();
extern Int32 get_base_dex();
extern Int32 get_base_con();
extern Int32 get_base_int();
extern Int32 get_base_wis();
extern Int32 get_base_chr();
extern Int32 get_base_mmana();
extern Int32 get_base_mhp();
extern Int32 get_base_weapon();
extern Int32 get_base_armour();
extern Int32 get_str();
extern Int32 get_dex();
extern Int32 get_con();
extern Int32 get_int();
extern Int32 get_wis();
extern Int32 get_chr();
extern Int32 get_mmana();
extern Int32 get_mhp();
extern Int32 get_weapon();
extern Int32 get_armour();
extern Int32 get_hpr();
extern Int32 get_manar();
extern Int32 get_hit();
extern double get_exp_multiplier();
extern Int32 get_magic_find();
extern double get_magic_resistance();
extern Int32 get_monster_level_plus();
extern Int32 get_crit();

extern void stats_do_damage(Int32 dam);
extern void stats_take_damage(Int32 dam);
extern void scale_monster(NPC_REC* monster, Int32 scale);
extern void charmmonster();
extern void set_title();
extern void make_assassin();
extern Int32 check_class_change(Int32 class);

extern void data_card(OBJECT_REC* it);
extern char* item_kind(OBJECT_REC* item);
extern void draw_item_card(OBJECT_REC* item,Int32 column,Int32 line);
extern void draw_card_panel(OBJECT_REC* item,Int32 top,Int32 bottom);
extern void shorten_name(char* name,Int32 width);
extern void list_item_name(OBJECT_REC* item,Int32 width,char* name);
extern char* base_name(char* buffer,OBJECT_REC* item);

extern Int32 get_skill(Int32 num);
extern void add_skill(Int32 skill,Int32 num);
extern void show_gain(Int32 g);
extern Int32 get_bonus(Int32 typ);
extern void fire_wand(Int32 damage);
extern void enchant(Int32 ench,Int32 limit);
extern void show_legends();
extern void mutable(OBJECT_REC* it);
extern Int32 item_color(OBJECT_REC* item);
extern void write_named(char* before,OBJECT_REC* item,char* name,char* after,Int32 color);
extern void itemColor(OBJECT_REC* item);
extern Int32 passable(Int32 x,Int32 y,char monstPass,char doorPass);
extern Int32 los(Int32 x1,Int32 y1,Int32 x2,Int32 y2,char monstPass);
extern void activate(Int32 x,Int32 y);
extern char adjust_map();
extern Int32 compass(Int32 suggest);
extern Int32 getPX(Int32 r,Int32 x);
extern Int32 getPY(Int32 r,Int32 y);
extern void level_legacy(OBJECT_REC* item);
extern Int32 mutable_enchant();
extern void check_level_legacy();
extern void player_attack_defense();
extern Int32 max_damage(OBJECT_REC* weapon,NPC_REC* target,Int32 base);
extern Int32 has_socket(Int32 intr);
extern Int32 figure_damage(Int32 damage,Int32 elem,NPC_REC* monster);
extern Int32 get_weight();
extern Int32 get_max_weight();
extern void blind_trap();
extern void root_trap();
extern void multiply_monster(NPC_REC* monster, Int32 scale);
extern Int32 can_see();
extern void target(Int32 sprt,Int32 stop_st_target,Int32 show_path);
char find_path(Int32 startx,Int32 starty,Int32 endx,Int32 endy,char ignoreMonst);
extern char fear_dir(Int32 start_x,Int32 start_y,Int32 end_x,Int32 end_y);
extern Int32 set_trap(Int32 n);
extern void arena_party_heal(Int32 healer);
extern void trigger_trap(Int32 x,Int32 y,Int32 trap_type);
extern NPC_REC* get_monster(Int32 x,Int32 y);
extern void do_heal(Int32 amount,Int32 who);
extern Int32 is_weapon(OBJECT_REC* item);
extern Int32 meleehit(Int32 who,OBJECT_REC* weapon,Int32 dmg,NPC_REC* target,char hand);
extern OBJECT_REC* get_item(Int32 x,Int32 y);
extern void thrown_potion(OBJECT_REC* item,Int32 x,Int32 y);
extern void aoe(Int32 cx,Int32 cy,Int32 effect,Int32 control,Int32 damage,Int32 element,Int32 range,Int32 mtype,Int32 toHit);
extern void use_item();
extern char valid_move(Int32 x,Int32 y);
extern void fire_weapon();
extern void do_get();
extern void throw(char hnd);
extern void cast_spell(Int32 number);
extern Int32 get_spell_power();
extern Int32 get_attack_power();
extern char* effect(Int32 e,Int32 c,char* buffer);
extern void get_attribute_name(char* buffer,Int32 ench);
extern Int32 has_trophy(Int32 num);
extern Int32 get_dodge();
extern void show_f_keys();
extern void get_shop_spells(Int32 level);
extern void show_spell(Int32 num,Int32 ox,Int32 oy,Int32 line);
extern void transcribe();
extern void mutable_set_name(OBJECT_REC* it);
extern Int32 get_gold();
extern Int32 get_coins();
extern void add_gold(Int32 g);
extern void sub_gold(Int32 g);
extern double typed_gold(double amount);
extern Int32 can_be_seen();
extern void enemy_hit(Int32 x,Int32 y);
extern void mutable_legendary(OBJECT_REC* item);
extern Int32 get_dungeon(Int32 n);
extern Int32 get_linked_msg(Int32 n);
extern void use_compass();
extern Int32 get_linked_monster(Int32 n);
extern void mapgen_old(Int32 floor);
extern void mapgen(Int32 depth,Int32 floor,Int32* treasurex,Int32* treasurey,
                   Int32 *upx, Int32 *upy, Int32 *downx, Int32 *downy,
                   MAP_TYPE* map,KNOW_TYPE* known);
extern Int32 damage_reduction(Int32 n);
extern Int32 magic_adjust(Int32 n);
extern void check_mount();
extern Int32 player_sprite(void);
extern Int32 rider_sprite(void);
extern Int32 reduce_damage(Int32 damage,Int32 element);
extern Int32 get_city();
extern double buy_price(double price);
extern Int32 triad_equipped();
extern Int32 fighting_final_boss(NPC_REC* mon);
extern Int32 offhand_weapon();
extern void show_time();
extern void process_config(char* name);
extern void save_config_value(char* name,char* key,Int32 value);
extern Int32 process_option(char* pChar);
extern void remember_built_in_defaults();
extern void remember_option_defaults();
extern void game_setup();
extern void game_options();
extern void read_character_rules();
extern void clear_gameplay_changes();
extern void save_gameplay_changes(FILE* file);
extern void load_gameplay_change(char* line);
extern char* mode_name(Int32 mode);
extern Int32 can_drop(Int32 destx,Int32 desty,char check_inair);
extern Int32 in_room(Int32 x,Int32 y);
extern void light_room();
extern void char_sheet_labels(char showall);
extern void char_sheet_data();
extern void show_skills();
extern void check_autos();
extern void Rcs_Seed(UInt32 s);
extern void Shift(Int32 count);
extern Int32 Rcs_Random(Int32 range);
extern void build_dungeon(Int32 floor,Int32 dn);
extern void save_level();
extern void load_level();
extern void save_gear(char weapons_only,Int32 slot);
extern void load_gear(char weapons_only,Int32 slot);
extern char item_dir(Int32 item);
extern void help_item(char* msg,Int32 col_width,Int32 num_rows,Int32 indent);
extern void convert_recipe(OBJECT_REC* item);
extern void sort_shop_items();
extern void get_shop_recipes();
extern Int32 check_level_cleared(Int32 lvl);
extern void pit_trap();
extern void fkey_pressed(Int32 n);
extern void shift_fkey_pressed(Int32 n);
extern void alt_key_pressed(Int32 n);
extern void ctrl_fkey_pressed(Int32 n);
extern Int32 find_empty(Int32 cx,Int32 cy,Int32 range,Int32 floor,Int32* dx,Int32 * dy);
extern void move_summoned();
extern Int32 find_target(Int32 x,Int32 y,Int32* tx,Int32* ty);
extern void save_new();
extern void save_game(char* name);
extern void write_line(FILE* file, char* line);
extern Int32 has_permadeath();
extern void hold_save(char* name);
extern void end_permadeath();
extern void clear_game_data();
extern char* next_word(char* buffer);
extern byte match(char* buffer, char* test);
extern Int32 possesses(Int32 link);
extern void hole_inventory();
extern void mech_merchant();


#ifdef ANNIV
extern unsigned short crc(unsigned char *s, Int32 len, unsigned short crcval);
#endif


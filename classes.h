/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#ifdef MAIN

/*  Name             Str Dex Con Int Wis Chr     evel 2  Next  Initial inventory slots           and items             HP MN BC   R_Skill   Rng  f_Skills       Ranges         FLG                                 SKT  Level Skill   Spl Hpr   weapon  sprite */
CLASS_REC classes[] = {
  { "Fighter",    {    0,  0,  0,  0,  0,  0 },     100, 1.20, { I_RHND, I_ARMR,      0,      0 }, { 299,353,  0,  0}, 10, 0, 0, { 0, 0, 0}, 0, { 2, 0, 0, 0}, {-5, 0, 0, 0 },CF_INITIAL,                            1, { 0, 0, 0, 0 }, 0, 0,   O_EDGE,  35 },    /* 0 */
  { "Paladin",    {    0,  0,  0,  0,  0,  0 },     175, 1.30, { I_RHND, I_ARMR,      0,      0 }, { 354,353,  0,  0}, 10, 5, 9, { 0, 0, 0}, 0, { 9,10, 4, 0}, {-3,-3,-5, 0 },CF_INITIAL|CF_PALADIN,                 2, { 0, 0, 0, 0 }, 4, 3,   O_BLNT,  35 },    /* 1 */
  { "Ranger",     {    0,  0,  0,  0,  0,  0 },     150, 1.25, { I_RHND, I_ARMR,      0,      0 }, { 355,356,  0,  0},  8, 5, 8, { 0, 0, 0}, 0, { 1, 5,21,15}, {-5,-5,-5,-3 },CF_INITIAL,                            3, { 0, 0, 0, 0 }, 5, 2,   O_BOW,   35 },    /* 2 */
  { "Cleric",     {    0,  0,  0,  0,  0,  0 },     125, 1.25, { I_RHND, I_ARMR, I_RSHL,      0 }, { 207,356, 72,  0},  6, 5, 1, { 0, 0, 0}, 0, { 4, 9, 0, 0}, {-5,-5, 0, 0 },CF_INITIAL|CF_CASTER,                  4, { 0, 0, 0, 0 }, 4, 0,   O_BLNT, 164 },    /* 3 */
  { "Druid",      {    0,  0,  0,  0,  0,  0 },     115, 1.25, { I_RHND, I_ARMR, I_RSHL,      0 }, { 358,356, 72,  0},  6, 5, 1, { 0, 0, 0}, 0, {23, 5,11, 0}, {-5,-4,-4, 0 },CF_INITIAL|CF_CASTER|CF_DRUID,         5, { 0, 0, 0, 0 }, 5, 2,   O_POLE, 165 },    /* 4 */
  { "Mage",       {    0,  0,  0,  0,  0,  0 },     200, 1.40, { I_RHND, I_ARMR, I_RSHL, I_LHND }, { 451,520, 72,357},  4, 5, 1, { 0, 0, 0}, 0, { 8, 0, 0, 0}, {-5, 0, 0, 0 },CF_INITIAL|CF_CASTER,                  6, { 0, 0, 0, 0 }, 6, 0,   O_WAND, 165 },    /* 5 */
  { "Monk",       {    0,  0,  0,  0,  0,  0 },     150, 1.30, { I_BLTL, I_BLTR, I_LHND,      0 }, { 499,499,523,  0},  4, 0, 0, { 0, 0, 0}, 0, { 9,24, 0, 0}, {-5,-5, 0, 0 },CF_INITIAL|CF_MONK,                    1, { 9,10,15,16 }, 0, 0,        0,  35 },    /* 6 */
  { "Assassin",   {    0,  0,  0,  0,  0,  0 },  100000, 1.30, {      0,      0,      0,      0 }, {   0,  0,  0,  0},  6, 0, 0, { 0, 0, 0}, 0, {17, 0, 0, 0}, {-5, 0, 0, 0 },CF_ROGUE,                              7, { 0, 0, 0, 0 }, 0, 0,   O_DAGR,  35 },    /* 7 */
  { "Hierophant", {    0,  0,  0,  0,  0,  0 },  500000, 1.30, {      0,      0,      0,      0 }, {   0,  0,  0,  0},  6, 5, 1, { 0, 0, 0}, 0, { 3, 0, 0, 0}, {-5, 0, 0, 0 },CF_HIER,                               5, { 0, 0, 0, 0 }, 5, 0,   O_POLE, 165 },    /* 8 */
  { "Cavalier",   {    0,  0,  0,  0,  0,  0 },  500000, 1.20, {      0,      0,      0,      0 }, {   0,  0,  0,  0}, 12, 0, 0, { 0, 0, 0}, 0, { 2, 3,11,16}, {-5, 5, 5, 5 },CF_CAVALIER,                           1, { 2, 3,15,16 }, 0, 0,   O_POLE, 163 },    /* 9 */
  { "Noble",      {    0,  0,  0,  0,  0,  0 }, 1000000, 1.30, {      0,      0,      0,      0 }, {   0,  0,  0,  0}, 10, 0, 0, { 0, 0, 0}, 0, { 2, 0, 0, 0}, {-5, 0, 0, 0 },CF_NOBLE,                              1, { 0, 0, 0, 0 }, 0, 0,   O_EDGE,  35 },    /* 10 */
  { "Enchanter",  {    0,  0,  0,  0,  0,  0 },     200, 1.40, { I_RHND, I_ARMR, I_RSHL,      0 }, { 299,356, 72,  0},  4, 5, 1, { 0, 0, 0}, 0, { 2, 0, 0, 0}, {-5, 0, 0, 0 },CF_INITIAL|CF_CASTER,                  6, { 0, 0, 0, 0 },12, 0,   O_EDGE, 165 },    /* 11 */
  { "Shaman",     {    0,  0,  0,  0,  0,  0 },     150, 1.30, { I_RHND, I_ARMR, I_RSHL,      0 }, { 207,356, 72,  0},  6, 5, 1, { 0, 0, 0}, 0, { 4, 9, 8,10}, {-5,-5,-5,-5 },CF_INITIAL|CF_CASTER,                  4, { 0, 0, 0, 0 },13, 2,   O_BLNT, 164 },    /* 12 */
  };

TRAITS class_traits[] = {
  {   7,     1,    A_NOPS },
  {   7,     1,    A_TRAP },
  {   7,     1,    A_NOFR },
  {   8,     1,    A_NOSL },
  {   8,     1,    A_NOPS },
  {   8,     1,    A_NOFR },
  {   9,     1,    A_TRAP },
  {  12,     1,    A_WATER },
  {  12,     6,    A_RCLD },
  {  12,     9,    A_RFIR },
  {  12,    12,    A_RELC },
  {  12,    15,    A_RELE },
  {  12,    18,    A_LEV },
  {  99,    99,        99 },
  };

CLASS_CHANGE_REC class_changes[] = {
/* From  Level  To */
  {   4,    23, 11 },        /* Cleric -> Noble */
  {   6,    38, 11 },        /* Mage -> Noble */
  {   7,    44, 11 },        /* Monk -> Noble */
  {   9,    31, 11 },        /* Hierophant -> Noble */
  {  10,    26, 11 },        /* Cavalier -> Noble */
  {   5,    31,  9 },        /* Druid -> Hierophant */
  {   1,    16, 10 },        /* Fighter -> Cavalier */
  {   2,    16, 10 },        /* Paladin -> Cavalier */
  {   3,    19, 10 },        /* Ranger -> Cavalier */
  {  12,    25, 11 },        /* Enchanter -> Noble */
  { 99,     99, 99 },
  };

TITLE_REC titles[] = {
  { 1,  1, "Veteran",                 "Veteran" },
  { 1,  2, "Warrior",                 "Warrior" },
  { 1,  5, "Swordsman",               "Swordmaiden" },
  { 1,  8, "Hero",                    "Heroine" },
  { 1, 11, "Swashbuckler",            "Swashbuckler" },
  { 1, 14, "Myrmidon",                "Myrmidon" },
  { 1, 17, "Champion",                "Champion" },
  { 1, 20, "Superhero",               "Superhero" },
  { 1, 23, "Lord",                    "Lady" },

  { 2,  1, "Gallant",                 "Gallant" },
  { 2,  2, "Keeper",                  "Keeper" },
  { 2,  5, "Protector",               "Protector" },
  { 2,  8, "Defender",                "Defender" },
  { 2, 11, "Warder",                  "Warder" },
  { 2, 14, "Guardian",                "Guardian" },
  { 2, 17, "Chevalier",               "Chevalier" },
  { 2, 20, "Justiciar",               "Justiciar" },
  { 2, 23, "Paladin",                 "Paladin" },

  { 3,  1, "Runner",                  "Runner" },
  { 3,  2, "Strider",                 "Strider" },
  { 3,  5, "Scout",                   "Scout" },
  { 3,  8, "Courser",                 "Courser" },
  { 3, 11, "Tracker",                 "Tracker" },
  { 3, 14, "Guide",                   "Guide" },
  { 3, 17, "Pathfinder",              "Pathfinder" },
  { 3, 20, "Ranger",                  "Ranger" },
  { 3, 23, "Ranger Knight",           "Ranger Knight" },
  { 3, 26, "Ranger Lord",             "Ranger Lord" },

  { 4,  1, "Acolyte",                 "Acolyte" },
  { 4,  2, "Adept",                   "Adept" },
  { 4,  5, "Priest",                  "Priestess" },
  { 4,  8, "Curate",                  "Curate" },
  { 4, 11, "Prefect",                 "Prefect" },
  { 4, 14, "Canon",                   "Canon" },
  { 4, 17, "Lama",                    "Lama" },
  { 4, 20, "Patriarch",               "Matriarch" },
  { 4, 23, "High Priest",             "High Priestess" },
  { 4, 26, "Prior",                   "Prior" },
  { 4, 29, "Abbot",                   "Abbot" },
  { 4, 32, "Bishop",                  "Bishop" },
  { 4, 35, "Archbishop",              "Archbishop" },

  { 5,  1, "Aspirant",                "Aspirant" },
  { 5,  2, "Ovate",                   "Ovate" },
  { 5,  5, "1st Circle",              "1st Circle" },
  { 5,  8, "2nd Circle",              "2nd Circle" },
  { 5, 11, "3rd Circle",              "3rd Circle" },
  { 5, 14, "4th Circle",              "4th Circle" },
  { 5, 17, "5th Circle",              "5th Circle" },
  { 5, 20, "6th Circle",              "6th Circle" },
  { 5, 23, "7th Circle",              "7th Circle" },
  { 5, 26, "8th Circle",              "8th Circle" },
  { 5, 29, "9th Circle",              "9th Circle" },
  { 5, 32, "Druid",                   "Druid" },
  { 5, 35, "Archdruid",               "Archdruid" },
  { 5, 38, "The Great Druid",         "The Great Druid" },

  { 6,  1, "Prestidigitator",         "Prestidigitator" },
  { 6,  2, "Evoker",                  "Evoker" },
  { 6,  5, "Conjurer",                "Conjurer" },
  { 6,  8, "Theurgist",               "Theurgist" },
  { 6, 11, "Thaumaturgist",           "Thaumaturgist" },
  { 6, 14, "Magician",                "Magician" },
  { 6, 17, "Enchanter",               "Enchantress" },
  { 6, 20, "Warlock",                 "Witch" },
  { 6, 23, "Sorcerer",                "Sorceress" },
  { 6, 26, "Necromancer",             "Necromancer" },
  { 6, 29, "Wizard",                  "Wizardess" },
  { 6, 32, "Mage",                    "Mage" },
  { 6, 35, "Grand Mage",              "Grand Mage" },
  { 6, 38, "Arch Mage",               "Arch Mage" },

  { 7,  1, "Novice",                  "Novice" },
  { 7,  2, "Initiate",                "Initiate" },
  { 7,  5, "Brother",                 "Sister" },
  { 7,  8, "Disciple",                "Disciple" },
  { 7, 11, "Immaculate",              "Immaculate" },
  { 7, 14, "Master",                  "Master" },
  { 7, 17, "Superior Master",         "Superior Master" },
  { 7, 20, "Master of Dragons",       "Master of Dragons" },
  { 7, 23, "Master of the N. Wind",   "Master of the N. Wind" },
  { 7, 26, "Master of the W. Wind",   "Master of the W. Wind" },
  { 7, 29, "Master of the S. Wind",   "Master of the S. Wind" },
  { 7, 32, "Master of the S. Wind",   "Master of the E. Wind" },
  { 7, 35, "Master of Winter",        "Master of Winter" },
  { 7, 38, "Master of Autumn",        "Master of Autumn" },
  { 7, 41, "Master of Summer",        "Master of Summer" },
  { 7, 44, "Master of Spring",        "Master of Spring" },
  { 7, 47, "Grand Master of Flowers", "Grand Master of Flowers" },

  { 8,  1, "Rogue",                   "Rogue" },
  { 8,  2, "Rutterkin",               "Rutterkin" },
  { 8,  5, "Waghalter",               "Waghalter" },
  { 8,  8, "Murderer",                "Murderer" },
  { 8, 11, "Thug",                    "Thug" },
  { 8, 14, "Killer",                  "Killer" },
  { 8, 17, "Cutthroat",               "Cutthroat" },
  { 8, 20, "Executioner",             "Executioner" },
  { 8, 23, "Assassin",                "Assassin" },
  { 8, 26, "Expert Assassin",         "Expert Assassin" },
  { 8, 29, "Senior Assassin",         "Senior Assassin" },
  { 8, 32, "Chief Assassin",          "Chief Assassin" },
  { 8, 35, "Prime Assassin",          "Prime Assassin" },
  { 8, 38, "Guildmaster Assassin",    "Guildmaster Assassin" },
  { 8, 41, "Grandfather of Assassins","Grandfather of Assassins" },

  { 9,  1, "Hierophant Druid",        "Hierophant Druid" },
  { 9,  4, "Hierophant Initiate",     "Hierophant Initiate" },
  { 9,  8, "Hierophant Adept",        "Hierophant Adept" },
  { 9, 12, "Hierophant Master",       "Hierophant Master" },
  { 9, 16, "Numinous Hierophant",     "Numinous Hierophant" },
  { 9, 21, "Mystic Hierophant",       "Mystic Hierophant" },
  { 9, 26, "Arcane Hierophant",       "Arcane Hierophant" },
  { 9, 31, "Hierophant of the Cabal", "Hierophant of the Cabal" },

  {10,  1, "Armiger",                 "Armiger" },
  {10,  2, "Scutifer",                "Scutifer" },
  {10,  5, "Esquire",                 "Esquire" },
  {10,  8, "Knight Errant",           "Knight Errant" },
  {10, 11, "Knight Bachelor",         "Knight Bachelor" },
  {10, 14, "Knight",                  "Knight" },
  {10, 17, "Grand Knight",            "Grand Knight" },
  {10, 20, "Bannert",                 "Bannert" },
  {10, 23, "Chevalier",               "Chevalier" },
  {10, 26, "Cavalier",                "Cavalier" },

  {11,  1, "Lord",                    "Lady" },
  {11,  2, "Baronet",                 "Baronet" },
  {11,  6, "Baron",                   "Baroness" },
  {11, 10, "Viscount",                "Viscountess" },
  {11, 14, "Count",                   "Countess" },
  {11, 18, "Marquis",                 "Marquise" },
  {11, 22, "Duke",                    "Duchess" },
  {11, 26, "Prince",                  "Princess" },
  {11, 30, "King of Halkanar",        "Queen of Halkanar" },
  {11, 34, "Emperor of Arne",         "Empress of Arne" },

  {12,  1, "Initiate Enchanter",      "Initiate Enchantress" },
  {12,  2, "Novice Enchanter",        "Novice Enchantress" },
  {12,  6, "Junior Enchanter",        "Junior Enchantress" },
  {12, 12, "Senior Enchanter",        "Senior Enchantress" },
  {12, 16, "Advanced Enchanter",      "Advanced Enchantress" },
  {12, 20, "Master Enchanter",        "Master Enchantress" },
  {12, 25, "Arcane Enchanter",        "Arcane Enchantress" },

  {13,  1, "Initiate Shaman",         "Initiate Shaman" },
  {13,  5, "Shaman",                  "Shaman" },
  {13,  9, "Master Shaman",           "Master Shaman" },
  {13, 13, "Master of Earth",         "Master of Earth" },
  {13, 17, "Master of Air",           "Master of Air" },
  {13, 21, "Master of Water",         "Master of Water" },
  {13, 25, "Master of Fire",          "Master of Fire" },
  {13, 30, "Master of the Elements",  "Master of the Elements" },

  {99, 99, "",                        "" },
  };

DEFAULT_SCORE_REC default_scores[] = {
  { "Elan of Marduk",       "Lord",                         836e3 },
  { "Gawaine of Radon",     "Paladin",                     3532e3 },
  { "Jason Thornblade",     "Ranger Lord",                 3841e3 },
  { "Amber of the Vale",    "High Priestess",              1573e3 },
  { "Kesthler of the Wood", "The Great Druid",           38963515 },
  { "Nina the Wise",        "Wizardess",                  48083e3 },
  { "Arslon",               "Grand Master of Flowers", 1494197000 },
  { "Galter the Feared",    "Grandfather of Assassins",  277837e4 },
  { "Rasper of Solat",      "Hierophant of the Cabal",   100769e4 },
  { "Ramoon of Talgon",     "Cavalier",                  39748460 },
  { "Marduk",               "King of Halkanar",        1550292000 },
  { "Syralia",              "Grand Enchantress",          48083e3 },
  { "Xundalu",              "Master of the Elements",     24700e3 },
  };

#else

extern CLASS_REC classes[];
extern CLASS_CHANGE_REC class_changes[];
extern TITLE_REC titles[];
extern DEFAULT_SCORE_REC default_scores[];
extern TRAITS class_traits[];

#endif

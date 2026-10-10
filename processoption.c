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

Int32 option_value(char* option) {
  while (*option != 0 && *option != '=') option++;
  if (*option == 0) return 0;
  option++;
  while (*option != 0 && *option == ' ') option++;
  if (*option == 0) return 0;
  return atoi(option);
  }

Int32 process_option(char* pChar) {
  Int32 ret;
  char option[1024];
  ret = -1;
  if (strlen(pChar) < 1) return ret;
  strcpy(option,pChar);
  if (option[0] == '-') {
    if (_stricmp(option,"-abd") == 0) auto_open_doors = 'Y';
    if (_stricmp(option,"-abw") == 0) auto_bump_walls = 'Y';
    if (_stricmp(option,"-ac") == 0) no_arena = 'N';
    if (_stricmp(option,"-aod") == 0) auto_open_doors = 'Y';
    if (_stricmp(option,"-apg") == 0) auto_pickup_gold = 'Y';
    if (_stricmp(option,"-apt") == 0) auto_pickup_torch = 'Y';
    if (_stricmp(option,"-asw") == 0) auto_bump_walls = 'Y';
    if (_stricmp(option,"-at") == 0) auto_target = 'Y';
    if (_stricmp(option,"-c") == 0) heroicMode = -1;
    if (_stricmp(option,"-e") == 0) heroicMode = 4;
    if (_stricmp(option,"-eds") == 0) dungeon_scaling = 'Y';
    if (_stricmp(option,"-fcd") == 0) party_combat_mode = 'F';
    if (_stricmp(option,"-h") == 0) heroicMode = 2;
    if (_stricmp(option,"-mu") == 0) music = 'Y';
    if (_stricmp(option,"-n") == 0) heroicMode = 0;
    if (_stricmp(option,"-nabd") == 0) auto_open_doors = 'N';
    if (_stricmp(option,"-nabw") == 0) auto_bump_walls = 'N';
    if (_stricmp(option,"-nac") == 0) no_arena = 'Y';
    if (_stricmp(option,"-naod") == 0) auto_open_doors = 'N';
    if (_stricmp(option,"-napg") == 0) auto_pickup_gold = 'N';
    if (_stricmp(option,"-napt") == 0) auto_pickup_torch = 'N';
    if (_stricmp(option,"-nasw") == 0) auto_bump_walls = 'N';
    if (_stricmp(option,"-nat") == 0) auto_target = 'N';
    if (_stricmp(option,"-nds") == 0) dungeon_scaling = 'N';
    if (_stricmp(option,"-nmu") == 0) music = 'N';
    if (_stricmp(option,"-nocrc") == 0) skipcheck = 'Y';
    if (_stricmp(option,"-rcd") == 0) party_combat_mode = 'R';
    if (_stricmp(option,"-s") == 0) showStats = -1;
    if (_stricmp(option,"-sk") == 0) show_keys = 'Y';
    if (_strnicmp(option,"-d=",3) == 0) {
      bottom = option_value(option);
      if (bottom < 2) bottom = 2;
      if (bottom > 10) bottom = 10;
      }
    if (_strnicmp(option,"-dh=",4) == 0) {
      delay_hit = option_value(option);
      }
    if (_strnicmp(option,"-dr=",4) == 0) {
      delay_ranged = option_value(option);
      }
    if (_strnicmp(option,"-dt=",4) == 0) {
      delay_trap = option_value(option);
      }
    if (_strnicmp(option,"-mc=",4) == 0) {
      monster_count = option_value(option);
      if (monster_count < 10) monster_count = 10;
      if (monster_count > 60) monster_count = 60;
      }
    if (_strnicmp(option,"-w=",3) == 0) {
      screen_width = option_value(option);
      }
    if (_strnicmp(option,"-h=",3) == 0) {
      screen_height = option_value(option);
      }
#ifdef MHR
#include "./Mhr/options1.c"
#endif
    }
  else {
         if (_stricmp(option,"ArenaCombat"       ) == 0) no_arena = 'N';
    else if (_stricmp(option,"AutoOpenDoors"     ) == 0) auto_open_doors = 'Y';
    else if (_stricmp(option,"AutoPickupGold"    ) == 0) auto_pickup_gold = 'Y';
    else if (_stricmp(option,"AutoPickupTorch"   ) == 0) auto_pickup_torch = 'Y';
    else if (_stricmp(option,"AutoSearchWalls"   ) == 0) auto_bump_walls = 'Y';
    else if (_stricmp(option,"AutoTarget"        ) == 0) auto_target = 'Y';
    else if (_stricmp(option,"Casual"            ) == 0) heroicMode = -1;
    else if (_stricmp(option,"DowngradeArena"    ) == 0) downgrade_arena = 'Y';
    else if (_stricmp(option,"DungeonScaling"    ) == 0) dungeon_scaling = 'Y';
    else if (_stricmp(option,"Epic"              ) == 0) heroicMode = 4;
    else if (_stricmp(option,"FkeyGear"          ) == 0) fkey_mode = 'G';
    else if (_stricmp(option,"FkeyMacros"        ) == 0) fkey_mode = 'M';
    else if (_stricmp(option,"FkeySpells"        ) == 0) fkey_mode = 'S';
    else if (_stricmp(option,"FkeyWeapons"       ) == 0) fkey_mode = 'W';
    else if (_stricmp(option,"FullPartyCombat"   ) == 0) party_combat_mode = 'F';
    else if (_stricmp(option,"GameThread"        ) == 0) game_thread = 'Y';
    else if (_stricmp(option,"Heroic"            ) == 0) heroicMode = 2;
    else if (_stricmp(option,"Music"             ) == 0) music = 'Y';
    else if (_stricmp(option,"NoArenaCombat"     ) == 0) no_arena = 'Y';
    else if (_stricmp(option,"NoAutoOpenDoors"   ) == 0) auto_open_doors = 'N';
    else if (_stricmp(option,"NoAutoPickupGold"  ) == 0) auto_pickup_gold = 'N';
    else if (_stricmp(option,"NoAutoPickupTorch" ) == 0) auto_pickup_torch = 'N';
    else if (_stricmp(option,"NoAutoSearchWalls" ) == 0) auto_bump_walls = 'Y';
    else if (_stricmp(option,"NoAutoTarget"      ) == 0) auto_target = 'N';
    else if (_stricmp(option,"NoDungeonScaling"  ) == 0) dungeon_scaling = 'N';
    else if (_stricmp(option,"NoFullPartyCombat" ) == 0) party_combat_mode = 'R';
    else if (_stricmp(option,"NoGameThread"      ) == 0) game_thread = 'N';
    else if (_stricmp(option,"NoMusic"           ) == 0) music = 'N';
    else if (_stricmp(option,"Normal"            ) == 0) heroicMode = 0;
    else if (_stricmp(option,"NoCrc"             ) == 0) skipcheck = 'Y';
    else if (_stricmp(option,"ShowKeys"          ) == 0) show_keys = 'Y';
    else if (_stricmp(option,"ShowStats"         ) == 0) showStats = -1;
#ifdef MHR
#include "./Mhr/options2.c"
#endif
    else if (_strnicmp(option,"DelayHit=",9) == 0) {
           delay_hit = option_value(option);
           }
    else if (_strnicmp(option,"DelayRanged=",12) == 0) {
           delay_ranged = option_value(option);
           }
    else if (_strnicmp(option,"DelayTraps=",11) == 0) {
           delay_trap = option_value(option);
           }
    else if (_strnicmp(option,"Depth=",6) == 0) {
           bottom = option_value(option);
           if (bottom < 2) bottom = 2;
           if (bottom > 10) bottom = 10;
           }
    else if (_strnicmp(option,"MonsterCount=",13) == 0) {
           monster_count = option_value(option);
           if (monster_count < 10) monster_count = 10;
           if (monster_count > 60) monster_count = 60;
           }
    else if (_strnicmp(option,"Width=",6) == 0) {
           screen_width = option_value(option);
           }
    else if (_strnicmp(option,"Height=",7) == 0) {
           screen_height = option_value(option);
           }
    else if (_strnicmp(option,"DowngradeDepth=",9) == 0) {
           downgrade_depth = option_value(option);
           if (downgrade_depth < 2) downgrade_depth = 2;
           }
    else if (_strnicmp(option,"M1=",3) == 0) strcpy(macros[0], option+3);
    else if (_strnicmp(option,"M2=",3) == 0) strcpy(macros[1], option+3);
    else if (_strnicmp(option,"M3=",3) == 0) strcpy(macros[2], option+3);
    else if (_strnicmp(option,"M4=",3) == 0) strcpy(macros[3], option+3);
    else if (_strnicmp(option,"M5=",3) == 0) strcpy(macros[4], option+3);
    else if (_strnicmp(option,"M6=",3) == 0) strcpy(macros[5], option+3);
    else if (_strnicmp(option,"M7=",3) == 0) strcpy(macros[6], option+3);
    else if (_strnicmp(option,"M8=",3) == 0) strcpy(macros[7], option+3);
    else if (_strnicmp(option,"M9=",3) == 0) strcpy(macros[8], option+3);
    else if (_strnicmp(option,"M10=",4) == 0) strcpy(macros[9], option+4);
    else if (_strnicmp(option,"M11=",4) == 0) strcpy(macros[10], option+4);
    else if (_strnicmp(option,"M12=",4) == 0) strcpy(macros[11], option+4);

/* *********************************************************** */
/* F options for macro definition is now depracted and will be */
/* removed in a future version                                 */
/* *********************************************************** */
    else if (_strnicmp(option,"F1=",3) == 0) strcpy(macros[0], option+3);
    else if (_strnicmp(option,"F2=",3) == 0) strcpy(macros[1], option+3);
    else if (_strnicmp(option,"F3=",3) == 0) strcpy(macros[2], option+3);
    else if (_strnicmp(option,"F4=",3) == 0) strcpy(macros[3], option+3);
    else if (_strnicmp(option,"F5=",3) == 0) strcpy(macros[4], option+3);
    else if (_strnicmp(option,"F6=",3) == 0) strcpy(macros[5], option+3);
    else if (_strnicmp(option,"F7=",3) == 0) strcpy(macros[6], option+3);
    else if (_strnicmp(option,"F8=",3) == 0) strcpy(macros[7], option+3);
    else if (_strnicmp(option,"F9=",3) == 0) strcpy(macros[8], option+3);
    else if (_strnicmp(option,"F10=",4) == 0) strcpy(macros[9], option+4);
    else if (_strnicmp(option,"F11=",4) == 0) strcpy(macros[10], option+4);
    else if (_strnicmp(option,"F12=",4) == 0) strcpy(macros[11], option+4);
    else ret = 0;
    }
  return ret;
  }


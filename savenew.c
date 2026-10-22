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

byte enc;

void write_line(FILE* file, char* line) {
  Int32 i;
  if (encrypt_save_file != 'N') {
    for (i=0; i<strlen(line); i++) {
      line[i] += enc;
      if ((byte)line[i] >= 127) line[i] -= 95;
      enc++;
      if (enc > 64) enc = 1;
      }
    }
  fprintf(file,"%s\n",line);
  }

void save_object(FILE* file, OBJECT_REC obj, Int32 indents) {
  Int32 i;
  char indent[64];
  char line[1024];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  sprintf(line,"%sLink %d",            indent, obj.link);             write_line(file, line);
  sprintf(line,"%sOType %d",           indent, obj.otype);            write_line(file, line);
  sprintf(line,"%sUserHp %d",          indent, obj.user_hp);          write_line(file, line);
  sprintf(line,"%sUserHealth %d",      indent, obj.user_health);      write_line(file, line);
  sprintf(line,"%sEffect %d",          indent, obj.effect);           write_line(file, line);
  sprintf(line,"%sControl %d",         indent, obj.control);          write_line(file, line);
  sprintf(line,"%sControl2 %d",        indent, obj.control2);         write_line(file, line);
  sprintf(line,"%sWeight %d",          indent, obj.weight);           write_line(file, line);
  sprintf(line,"%sCost %f",            indent, obj.cost);             write_line(file, line);
  sprintf(line,"%sLevel %d",           indent, obj.level);            write_line(file, line);
  sprintf(line,"%sDamage %d",          indent, obj.damage);           write_line(file, line);
  sprintf(line,"%sSprite %d",          indent, obj.sprite);           write_line(file, line);
  sprintf(line,"%sRarity %d",          indent, obj.rarity);           write_line(file, line);
  sprintf(line,"%sPlus %d",            indent, obj.plus);             write_line(file, line);
  sprintf(line,"%sCharges %d",         indent, obj.charges);          write_line(file, line);
  sprintf(line,"%sCostP %f",           indent, obj.costp);            write_line(file, line);
  sprintf(line,"%sAlign %d",           indent, obj.align);            write_line(file, line);
  sprintf(line,"%sSkill %d",           indent, obj.skill);            write_line(file, line);
  sprintf(line,"%sMinSkill %d",        indent, obj.min_skill);        write_line(file, line);
  sprintf(line,"%sName %s",            indent, obj.name);             write_line(file, line);
  sprintf(line,"%sNumEnchantments %d", indent, obj.num_enchantments); write_line(file, line);
  sprintf(line,"%sNumSockets %d",      indent, obj.num_sockets);      write_line(file, line);
  sprintf(line,"%sFlags %u",           indent, obj.flags);            write_line(file, line);
  sprintf(line,"%sFlags2 %u",          indent, obj.flags2);           write_line(file, line);
  for (i=0; i<obj.num_enchantments; i++) {
    sprintf(line,"%sEnchant%d %d", indent, i+1, obj.enchantments[i]);
    write_line(file, line);
    }
  for (i=0; i<obj.num_sockets; i++) {
    sprintf(line,"%sSocket%d %d", indent, i+1, obj.sockets[i]);
    write_line(file, line);
    }
  sprintf(line,"%s}", indent); write_line(file, line);
  }

void save_npc(FILE* file, NPC_REC npc, Int32 indents) {
  char indent[64];
  char line[1024];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  sprintf(line,"%sSpeed %d",            indent, npc.speed);      write_line(file, line);
  sprintf(line,"%sNumber %d",           indent, npc.number);     write_line(file, line);
  sprintf(line,"%sHp %d",               indent, npc.hp);         write_line(file, line);
  sprintf(line,"%sMaxHp %d",            indent, npc.max_hp);     write_line(file, line);
  sprintf(line,"%sMana %d",             indent, npc.mana);       write_line(file, line);
  sprintf(line,"%sLevel %d",            indent, npc.level);      write_line(file, line);
  sprintf(line,"%sAttack %d",           indent, npc.attack);     write_line(file, line);
  sprintf(line,"%sDefense %d",          indent, npc.defense);    write_line(file, line);
  sprintf(line,"%sExp %f",              indent, npc.exp);        write_line(file, line);
  sprintf(line,"%sExpHp %f",            indent, npc.exp_hp);     write_line(file, line);
  sprintf(line,"%sspecial %u",          indent, npc.special);    write_line(file, line);
  sprintf(line,"%sspecial2 %u",         indent, npc.special2);   write_line(file, line);
  sprintf(line,"%sSprite %d",           indent, npc.sprite);     write_line(file, line);
  sprintf(line,"%sMType %d",            indent, npc.mtype);      write_line(file, line);
  sprintf(line,"%sName %s",             indent, npc.name);       write_line(file, line);
  sprintf(line,"%sTime %d",             indent, npc.time);       write_line(file, line);
  sprintf(line,"%sSleep %d",            indent, npc.sleep);      write_line(file, line);
  sprintf(line,"%sFear %d",             indent, npc.fear);       write_line(file, line);
  sprintf(line,"%sBlind %d",            indent, npc.blind);      write_line(file, line);
  sprintf(line,"%sConfused %d",         indent, npc.confused);   write_line(file, line);
  sprintf(line,"%sPoison %d",           indent, npc.poison);     write_line(file, line);
  sprintf(line,"%sPrev %d",             indent, npc.prev);       write_line(file, line);
  sprintf(line,"%sTreasure %d",         indent, npc.treasure);   write_line(file, line);
  sprintf(line,"%s}", indent); write_line(file, line);
  }

void save_player(FILE* file) {
  Int32 i, j;
  char line[1024];
  sprintf(line, "Player {"); write_line(file, line);
  sprintf(line, "  Name %s",player.name);              write_line(file, line);
  sprintf(line, "  Age %d",player.age);                write_line(file, line);
  sprintf(line, "  Sex %c",player.sex);                write_line(file, line);
  sprintf(line, "  Level %d",player.level);            write_line(file, line);
  sprintf(line, "  Exp %f",player.exp);                write_line(file, line);
  sprintf(line, "  ExpNext %f",player.exp_next);       write_line(file, line);
  sprintf(line, "  Class %d",player.class);            write_line(file, line);
  sprintf(line, "  Title %s",player.title);            write_line(file, line);
  sprintf(line, "  Race %d",player.race);              write_line(file, line);
  for (i=0; i<7; i++) {
    sprintf(line, "  Stat%d %d",i, player.stat[i]); write_line(file, line);
    }
  sprintf(line, "  MHp %d",player.mhp);                write_line(file, line);
  sprintf(line, "  CHp %d",player.chp);                write_line(file, line);
  sprintf(line, "  Health %d",player.health);          write_line(file, line);
  sprintf(line, "  MMana %d",player.mmana);            write_line(file, line);
  sprintf(line, "  CMana %d",player.cmana);            write_line(file, line);
  for (i=0; i<11; i++) {
    sprintf(line, "  Ability%d %d",i, player.ability[i]); write_line(file, line);
    }
  sprintf(line, "  X %d",player.x);                    write_line(file, line);
  sprintf(line, "  Y %d",player.y);                    write_line(file, line);
  sprintf(line, "  MapX %d",player.map_x);             write_line(file, line);
  sprintf(line, "  MapY %d",player.map_y);             write_line(file, line);
  sprintf(line, "  Region %d",player.region);          write_line(file, line);
  for (i=0; i<31; i++) {
    if (player.skill[i] != 0) {
      sprintf(line, "  Skill%d %d",i, player.skill[i]); write_line(file, line);
      }
    }
  sprintf(line, "  Torch %d",player.torch);            write_line(file, line);
  sprintf(line, "  Food %d",player.food);              write_line(file, line);
  sprintf(line, "  Armor %d",player.armour);           write_line(file, line);
  sprintf(line, "  Weapon %d",player.weapon);          write_line(file, line);
  sprintf(line, "  Mount %d",player.mount);            write_line(file, line);
  sprintf(line, "  Peg %d",player.peg);                write_line(file, line);
  sprintf(line, "  Gold %d",player.gold_coins);             write_line(file, line);
  sprintf(line, "  Mithril %d",player.mithril_coins);       write_line(file, line);
  sprintf(line, "  Adamantite %d",player.adamantite_coins); write_line(file, line);
  sprintf(line, "  Bank %d",player.bank);              write_line(file, line);
  sprintf(line, "  Interest %d",player.interest);      write_line(file, line);
  sprintf(line, "  DLevel %d",player.d_level);         write_line(file, line);
  sprintf(line, "  Diff %d",player.diff);              write_line(file, line);
  sprintf(line, "  LandX %d",player.land_x);           write_line(file, line);
  sprintf(line, "  LandY %d",player.land_y);           write_line(file, line);
  sprintf(line, "  DName %s",player.d_name);           write_line(file, line);
  sprintf(line, "  Day %d",player.day);                write_line(file, line);
  sprintf(line, "  Month %d",player.month);            write_line(file, line);
  sprintf(line, "  Min %d",player.min);                write_line(file, line);
  sprintf(line, "  Hour %d",player.hour);              write_line(file, line);
  for (i=0; i<255; i++)
    if (player.effect[i] != 0) {
      sprintf(line,"  Effect%d %d", i, player.effect[i]); write_line(file, line);
      }
  sprintf(line, "  Align %d",player.align);            write_line(file, line);
  sprintf(line, "  InCity %d",player.in_city);         write_line(file, line);
  sprintf(line, "  City %d",player.city);              write_line(file, line);
  sprintf(line, "  HpCount %d",player.hp_count);       write_line(file, line);
  sprintf(line, "  ManaCount %d",player.mana_count);   write_line(file, line);
  sprintf(line, "  Phase %d",player.phase);            write_line(file, line);
  sprintf(line, "  ShipX %d",player.ship_x);           write_line(file, line);
  sprintf(line, "  ShipY %d",player.ship_y);           write_line(file, line);
  sprintf(line, "  ShipR %d",player.ship_r);           write_line(file, line);
  sprintf(line, "  InShip %d",player.inship);          write_line(file, line);
  sprintf(line, "  Gateway %d",player.gateway);        write_line(file, line);
  sprintf(line, "  CavPaid %d",player.cav_paid);       write_line(file, line);
  sprintf(line, "  DungeonNum %d",player.dungeon_number); write_line(file, line);
  sprintf(line, "  TimePlayed %d",player.time_played); write_line(file, line);
  for (i=0; i<11; i++)
    if (player.tardis_x[i] != 0 || player.tardis_y[i] != 0) {

      /* Without the Tardis Saves change a nested block, which load_player() misreads */
      if (change_tardis_saves == 'Y') {
        sprintf(line, "  TardisX%d %d",i,player.tardis_x[i]);    write_line(file, line);
        sprintf(line, "  TardisY%d %d",i,player.tardis_y[i]);    write_line(file, line);
        sprintf(line, "  TardisR%d %d",i,player.tardis_r[i]);    write_line(file, line);
        sprintf(line, "  TardisDesc%d %s",i,player.tardis_d[i]); write_line(file, line);
        }
      else {
        sprintf(line, "  Tardis %d {",i);             write_line(file, line);
        sprintf(line, "    X %d",player.tardis_x[i]); write_line(file, line);
        sprintf(line, "    Y %d",player.tardis_y[i]); write_line(file, line);
        sprintf(line, "    R %d",player.tardis_r[i]); write_line(file, line);
        sprintf(line, "    Desc %s",player.tardis_d[i]); write_line(file, line);
        sprintf(line, "    }"); write_line(file, line);
        }
      }
  for (i=0; i<5000; i++)
    if (player.gen[i] != 0) {
      sprintf(line, "  Gen%d %d",i, player.gen[i]); write_line(file, line);
      }
  for (i=0; i<5000; i++)
    if (player.lore[i] != 0) {
      sprintf(line, "  Lore%d %d",i, player.lore[i]); write_line(file, line);
      }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "Inventory {"); write_line(file, line);
  for (i=0; i<20; i++)
    if (player.invent[i].otype != O_NONE) {
      sprintf(line, "  Slot %d {",i); write_line(file, line);
      save_object(file, player.invent[i], 4);
      }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "Pack {"); write_line(file, line);
  for (i=0; i<33; i++)
    if (player.pack[i].otype != O_NONE) {
      sprintf(line, "  Slot %d {",i); write_line(file, line);
      save_object(file, player.pack[i], 4);
      }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "PortableHole {"); write_line(file, line);
  for (i=0; i<1000; i++)
    if (player.hole[i].otype != O_NONE) {
      sprintf(line, "  Slot %d {",i); write_line(file, line);
      save_object(file, player.hole[i], 4);
      }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "Party {"); write_line(file, line);
  for (i=0; i<6; i++)
    if (player.party[i].hp != 0) {
      sprintf(line, "  Slot %d {",i); write_line(file, line);
      save_npc(file, player.party[i], 4);
      }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "CompletedQuests {"); write_line(file, line);
  for (i=0; i<1000; i++)
    if (player.quests[i] == 'Y') {
      sprintf(line,"  Quest %d",i); write_line(file, line);
      }

  /* Without the Quest State change only completed quests are saved, so entered ('E') dungeons are lost on a load */
  if (change_quest_state == 'Y')
    for (i=0; i<1000; i++)
      if (player.quests[i] == 'E') {
        sprintf(line,"  Entered %d",i); write_line(file, line);
        }
  sprintf(line, "  }"); write_line(file, line);
  sprintf(line, "FKeySpells {"); write_line(file, line);
  for (i=0; i<13; i++)
    if (strlen(player.fspell[i]) > 0) {
      sprintf(line,"  Spell%d %s",i,player.fspell[i]); write_line(file, line);
      }
  sprintf(line, "  }"); write_line(file, line);
  for (i=0; i<5; i++) {
    sprintf(line, "House %d {",i); write_line(file, line);
    sprintf(line, "  Region %d",player.houses[i].region); write_line(file, line);
    sprintf(line, "  RegionX %d",player.houses[i].rx); write_line(file, line);
    sprintf(line, "  RegionY %d",player.houses[i].ry); write_line(file, line);
    sprintf(line, "  CityX %d",player.houses[i].cx); write_line(file, line);
    sprintf(line, "  CityY %d",player.houses[i].cy); write_line(file, line);
    sprintf(line, "  Mortgage %f",player.houses[i].mortgage); write_line(file, line);
    sprintf(line, "  Payment %f",player.houses[i].payment); write_line(file, line);
    sprintf(line, "  Interest %f",player.houses[i].interest); write_line(file, line);
    for (j=0; j<500; j++)
      if (player.houses[i].inventory[j].otype != O_NONE) {
        sprintf(line, "  Slot %d {",j); write_line(file, line);
        save_object(file, player.houses[i].inventory[j], 4);
        }
    sprintf(line, "  }"); write_line(file, line);
    }
  }

/* Options that are not already player.effect[] entries. */
void save_options(FILE* file) {
  char line[1024];
  sprintf(line, "Options {");                               write_line(file, line);
  sprintf(line, "  Permadeath %c", permadeath);             write_line(file, line);
  sprintf(line, "  DungeonScaling %c", dungeon_scaling);    write_line(file, line);
  sprintf(line, "  PartyCombat %c", party_combat_mode);     write_line(file, line);
  sprintf(line, "  AutoTarget %c", auto_target);            write_line(file, line);
  sprintf(line, "  AutoPickupGold %c", auto_pickup_gold);   write_line(file, line);
  sprintf(line, "  AutoPickupTorch %c", auto_pickup_torch); write_line(file, line);
  sprintf(line, "  AutoOpenDoors %c", auto_open_doors);     write_line(file, line);
  sprintf(line, "  AutoSearchWalls %c", auto_bump_walls);   write_line(file, line);
  sprintf(line, "  DelayRanged %d", delay_ranged);          write_line(file, line);
  sprintf(line, "  DelayHit %d", delay_hit);                write_line(file, line);
  sprintf(line, "  DelayTraps %d", delay_trap);             write_line(file, line);
  sprintf(line, "  EnhancedSprites %c", enhanced_sprites);  write_line(file, line);
  sprintf(line, "  ShowMount %c", show_mount);              write_line(file, line);
  sprintf(line, "  SeamlessWorldMap %c", seamless_world_map); write_line(file, line);
  sprintf(line, "  FollowCamera %c", follow_camera);        write_line(file, line);
  sprintf(line, "  UniqueClassSprites %c", class_sprites);  write_line(file, line);
  sprintf(line, "  FloorItemNames %c", floor_item_names);   write_line(file, line);
  sprintf(line, "  ImprovedItems %c", improved_items);      write_line(file, line);
  sprintf(line, "  ImprovedCombatLogs %c", improved_combat_logs); write_line(file, line);
  sprintf(line, "  GothicScrolls %c", gothic_scrolls);      write_line(file, line);
  sprintf(line, "  ImprovedInterface %c", improved_interface); write_line(file, line);
  sprintf(line, "  ImprovedStatus %c", improved_status);    write_line(file, line);
  sprintf(line, "  EasySpells %c", easy_spells);            write_line(file, line);
  sprintf(line, "  EasyWords %c", easy_words);              write_line(file, line);
  save_gameplay_changes(file);
  sprintf(line, "  }");                                     write_line(file, line);
  }

void save_known(FILE* file, KNOW_TYPE known, Int32 indents) {
  Int32 x, y;
  Int32 count;
  char indent[64];
  char line[1024];
  char outp[1024];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  
  sprintf(line,"%sKnown {",            indent);      write_line(file, line);
  count = 0;
  for (y=0; y<101; y++)
    for (x=0; x<101; x++) {
    line[count++] = (known[x][y] == 0) ? '0' : '1';
    line[count] = 0;
    if (count == 70) {
      sprintf(outp,"%s  %s",indent, line); write_line(file,outp);
      count = 0;
      }
    }
  if (count != 0) {
    sprintf(outp,"%s  %s",indent, line); write_line(file,outp);
    }
  sprintf(line,"%s  }",indent); write_line(file, line);
  }

void save_map(FILE* file, MAP_TYPE map, Int32 indents) {
  Int32 x, y;
  char indent[64];
  char line[1024];
  char outp[1024];
  char num[32];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  sprintf(line,"%sMap {",            indent);      write_line(file, line);
  strcpy(line, "");
  for (y=0; y<101; y++)
    for (x=0; x<101; x++) {
      sprintf(num," %d", map[x][y]);
      strcat(line, num);
      if (strlen(line) >=70) {
        sprintf(outp,"%s  %s",indent, line); write_line(file,outp);
        strcpy(line, "");
        }
      }
  if (strlen(line) > 0) {
    sprintf(outp,"%s  %s",indent, line); write_line(file,outp);
    }
  sprintf(line,"%s  }",indent); write_line(file, line);
  }

void save_monsters(FILE* file, DUNGEON_MONSTERS monsters, Int32 indents) {
  Int32 p, m;
  char indent[64];
  char line[1024];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  for (p=0; p<61; p++)
    if (monsters[1][p].hp > 0) {
      sprintf(line, "%sGroup %d {",indent,p);
      write_line(file, line);
      for (m=0; m<6; m++)
        if (monsters[m][p].hp > 0) {
          sprintf(line, "%s  Monster %d {",indent, m); write_line(file, line);
          save_npc(file,monsters[m][p], indents+4);
          }
      sprintf(line, "%s  }",indent); write_line(file, line);
      }
  }

void save_objects(FILE* file, DUNGEON_OBJECTS objects, Int32 indents) {
  Int32 i;
  char indent[64];
  char line[1024];
  strcpy(indent,"                                ");
  indent[indents] = 0;
  for (i=0; i<1000; i++)
    if (objects[i].otype != O_NONE) {
      sprintf(line, "%sLoot %d {",indent,i); write_line(file, line);
      save_object(file, objects[i], indents + 2);
      }
  }

void save_current_level(FILE* file) {
  char line[1024];
  sprintf(line,"CurrentLevel {"); write_line(file, line);
  save_monsters(file, player.monster, 2);
  save_objects(file, player.map_objects, 2);
  save_known(file, player.known, 2);
  save_map(file, player.map, 2);
  sprintf(line, "  }"); write_line(file, line);
  }

void save_pawn_shops(FILE* file) {
  Int32 city;
  Int32 slot;
  char line[1024];
  for (city=0; city < 10; city++) {
    sprintf(line, "PawnShop %d {", city); write_line(file, line);
    for (slot=0; slot<11; slot++)
      if (player.pawn[city][slot].otype != O_NONE) {
        sprintf(line, "  Slot %d {",slot); write_line(file, line);
        save_object(file, player.pawn[city][slot], 4);
        }
    sprintf(line, "  }"); write_line(file, line);
    }
  }

void save_stats(FILE* file) {
  Int32 i;
  char line[1024];
  sprintf(line,"Stats {"); write_line(file, line);
  for (i=0; i<255; i++)
    if (stats.counts[i] != 0) {
      sprintf(line,"  Count%d %d",i,stats.counts[i]); write_line(file, line);
      }
  for (i=0; i<stats.num_trophies; i++) {
    sprintf(line, "  Trophy %d", stats.trophies[i]); write_line(file, line);
    }
  sprintf(line,"  }"); write_line(file,line);
  }

void save_spellbook(FILE* file) {
  Int32 i;
  char line[1024];
  sprintf(line,"Spellbook {"); write_line(file, line);
  for (i=0; i<1000; i++)
    if (spellbook[i] != 0) {
      sprintf(line,"  Spell %d",spellbook[i]); write_line(file, line);
      }
  sprintf(line,"  }"); write_line(file,line);
  }

void save_gear_manager(FILE* file) {
  Int32 i;
  char line[1024];
  sprintf(line,"GearManager {"); write_line(file, line);
  for (i=0; i<10; i++) {
    sprintf(line,"  Set %d {",i); write_line(file, line);
    if (gear_manager[i].main_hand[0] != 0) {
      sprintf(line,"    MainHand %s",gear_manager[i].main_hand); write_line(file, line);
      }
    if (gear_manager[i].off_hand[0] != 0) {
      sprintf(line,"    OffHand %s",gear_manager[i].off_hand); write_line(file, line);
      }
    if (gear_manager[i].neck_1[0] != 0) {
      sprintf(line,"    Neck1 %s",gear_manager[i].neck_1); write_line(file, line);
      }
    if (gear_manager[i].neck_2[0] != 0) {
      sprintf(line,"    Neck2 %s",gear_manager[i].neck_2); write_line(file, line);
      }
    if (gear_manager[i].shield[0] != 0) {
      sprintf(line,"    Shield %s",gear_manager[i].shield); write_line(file, line);
      }
    if (gear_manager[i].armor[0] != 0) {
      sprintf(line,"    Armor %s",gear_manager[i].armor); write_line(file, line);
      }
    if (gear_manager[i].cloak[0] != 0) {
      sprintf(line,"    Cloak %s",gear_manager[i].cloak); write_line(file, line);
      }
    if (gear_manager[i].boots[0] != 0) {
      sprintf(line,"    Boots %s",gear_manager[i].boots); write_line(file, line);
      }
    if (gear_manager[i].helm[0] != 0) {
      sprintf(line,"    Helm %s",gear_manager[i].helm); write_line(file, line);
      }
    if (gear_manager[i].ring_l1[0] != 0) {
      sprintf(line,"    RingL1 %s",gear_manager[i].ring_l1); write_line(file, line);
      }
    if (gear_manager[i].ring_l2[0] != 0) {
      sprintf(line,"    RingL2 %s",gear_manager[i].ring_l2); write_line(file, line);
      }
    if (gear_manager[i].ring_r1[0] != 0) {
      sprintf(line,"    RingR1 %s",gear_manager[i].ring_r1); write_line(file, line);
      }
    if (gear_manager[i].ring_r2[0] != 0) {
      sprintf(line,"    RingR2 %s",gear_manager[i].ring_r2); write_line(file, line);
      }
    sprintf(line,"    }"); write_line(file, line);
    }
  sprintf(line,"  }"); write_line(file,line);
  }

void save_known_maps(FILE* file) {
  Int32 i;
  Int32 x,y;
  char flag;
  char line[1024];
  sprintf(line,"KnownMaps {"); write_line(file, line);
  for (i=0; i<=NO_OF_MAPS; i++) {
    flag = 0;
    for (y=0; y<=100; y++)
      for (x=0; x<=100; x++)
        if (known_maps[i][x][y] != 0) {
          flag = 0xff;
          x = 100;
          y = 100;
          }
    if (flag) {
      sprintf(line,"  Map %d {",i); write_line(file, line);
      save_known(file, known_maps[i], 4);
      sprintf(line,"    }"); write_line(file, line);
      }
    }
  sprintf(line,"  }"); write_line(file,line);
  }

void save_current_dungeon(FILE* file) {
  Int32 i;
  char line[1024];
  sprintf(line,"CurrentDungeon {"); write_line(file, line);
  sprintf(line, "  NumLevels %d",current_dungeon.num_levels); write_line(file, line);
  sprintf(line, "  Number %d",current_dungeon.dungeon_number); write_line(file, line);
  sprintf(line, "  TreasureX %d",current_dungeon.treasurex); write_line(file, line);
  sprintf(line, "  TreasureY %d",current_dungeon.treasurey); write_line(file, line);
  for (i=0; i<current_dungeon.num_levels; i++) {
    sprintf(line, "  Level %d {",i); write_line(file, line);
    sprintf(line, "    UpX %d", current_dungeon.level[i].upx); write_line(file, line);
    sprintf(line, "    UpY %d", current_dungeon.level[i].upy); write_line(file, line);
    sprintf(line, "    DownX %d", current_dungeon.level[i].downx); write_line(file, line);
    sprintf(line, "    DownY %d", current_dungeon.level[i].downy); write_line(file, line);
    sprintf(line, "    Cleared %d", current_dungeon.level[i].cleared); write_line(file, line);
    save_known(file, current_dungeon.level[i].known, 4);
    save_map(file, current_dungeon.level[i].map, 4);
    save_monsters(file, current_dungeon.level[i].monsters, 4);
    save_objects(file, current_dungeon.level[i].objects, 4);
    sprintf(line, "    }"); write_line(file, line);
    }
  sprintf(line,"  }"); write_line(file,line);
  }

void save_new() {
  char  filename[1024];
  gotoxy(1,1); textcolor(14);
  strcpy(filename,"");
  while (strlen(filename)<=1 || strlen(filename) >= sizeof(backupname) - 4) {
    strcpy(filename,loadname);
    if (strlen(filename) == 0) strcpy(filename,player.name);
    gotoxy(1,1); textcolor(14);
    write_question("Filename to save under (No Extension)");
    if (improved_interface == 'Y') txt_write(" ");
    readln(filename,sizeof(filename));
    }
  save_game(filename);
  }

/* Writes the game to <name>.sav, which becomes the game's save */
void save_game(char* name) {
  FILE* file;
  char  filename[1024];
  if (name != loadname) strcpy(loadname, name);
  strcpy(filename, name);
  strcat(filename, ".sav");
  file = fopen(filename, "w");
  if (encrypt_save_file != 'N') fprintf(file, "%se\n",VERSION);
    else fprintf(file, "%s\n",VERSION);
  enc = 1;
  save_player(file);
  save_options(file);
  save_pawn_shops(file);
  save_current_level(file);
  save_known_maps(file);
  save_current_dungeon(file);
  save_stats(file);
  save_spellbook(file);
  save_gear_manager(file);
  fclose(file);
  }


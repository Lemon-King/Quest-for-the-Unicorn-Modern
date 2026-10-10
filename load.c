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

byte dbuffer[1024];
Int32  dbuffer_pos;
Int32  dbuffer_count;
FILE*  save_file;
byte eosf;
byte encrypted;
byte offset;
KNOW_TYPE known;
MAP_TYPE  map;

UInt32 atoui(char* buffer) {
  UInt32 ret;
  ret = 0;
  while (*buffer >= '0' && *buffer <= '9') {
    ret *= 10;
    ret += (*buffer - '0');
    buffer++;
    }
  return ret;
  }

byte read_byte() {
  if (dbuffer_pos < dbuffer_count) return dbuffer[dbuffer_pos++];
  dbuffer_count = (Int32)fread(dbuffer, 1, sizeof(dbuffer), save_file);
  dbuffer_pos = 0;
  if (dbuffer_pos < dbuffer_count) return dbuffer[dbuffer_pos++];
  eosf = 0xff;
  return 0;
  }

byte decrypt(byte value) {
  if (encrypted == 0) return value;
  if (value < 32) return value;
  value += 64;
  value -= offset;
  if (value < 95) value += 95;
  value -= 64;
  if (++offset > 64) offset = 1;
  return value;
  }

void read_line(char* buffer) {
  byte value;
  value = decrypt(read_byte());
  while (value > 0 && value <= ' ') value = decrypt(read_byte());
  while (value >= ' ') {
    *buffer++ = value;
    value = decrypt(read_byte());
    }
  *buffer = 0;
  }

char* next_word(char* buffer) {
  while (*buffer > 0 && *buffer != ' ') buffer++;
  while (*buffer == ' ') buffer++;
  return buffer;
  }

byte match(char* buffer, char* test) {
  if (_strnicmp(buffer, test, strlen(test)) == 0) return 0xff;
  return 0;
  }

OBJECT_REC load_object() {
  OBJECT_REC ret;
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  memset(&ret, 0, sizeof(OBJECT_REC));
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Link ")) { next = next_word(line); ret.link = atoi(next); }
      if (match(line, "OType ")) { next = next_word(line); ret.otype = atoi(next); }
      if (match(line, "UserHp ")) { next = next_word(line); ret.user_hp = atoi(next); }
      if (match(line, "UserHealth ")) { next = next_word(line); ret.user_health = atoi(next); }
      if (match(line, "Effect ")) { next = next_word(line); ret.effect = atoi(next); }
      if (match(line, "Control ")) { next = next_word(line); ret.control = atoi(next); }
      if (match(line, "Control2 ")) { next = next_word(line); ret.control2 = atoi(next); }
      if (match(line, "Weight ")) { next = next_word(line); ret.weight = atoi(next); }
      if (match(line, "Cost ")) { next = next_word(line); ret.cost = atof(next); }
      if (match(line, "Level ")) { next = next_word(line); ret.level = atoi(next); }
      if (match(line, "Damage ")) { next = next_word(line); ret.damage = atoi(next); }
      if (match(line, "Sprite ")) { next = next_word(line); ret.sprite = atoi(next); }
      if (match(line, "Rarity ")) { next = next_word(line); ret.rarity = atoi(next); }
      if (match(line, "Plus ")) { next = next_word(line); ret.plus = atoi(next); }
      if (match(line, "Charges ")) { next = next_word(line); ret.charges = atoi(next); }
      if (match(line, "CostP ")) { next = next_word(line); ret.costp = atof(next); }
      if (match(line, "Align ")) { next = next_word(line); ret.align = atoi(next); }
      if (match(line, "Skill ")) { next = next_word(line); ret.skill = atoi(next); }
      if (match(line, "MinSkill ")) { next = next_word(line); ret.min_skill = atoi(next); }
      if (match(line, "Name ")) { next = next_word(line); strcpy(ret.name, next); }
      if (match(line, "NumEnchantments ")) { next = next_word(line); ret.num_enchantments = atoi(next); }
      if (match(line, "NumSockets ")) { next = next_word(line); ret.num_sockets = atoi(next); }
      if (match(line, "Flags ")) { next = next_word(line); ret.flags = atoi(next); }
      if (match(line, "Flags2 ")) { next = next_word(line); ret.flags2 = atoi(next); }
      if (match(line, "Enchant")) { 
        i = atoi(line+7);
        next = next_word(line); ret.enchantments[i-1] = atoi(next);
        }
      if (match(line, "Socket")) { 
        i = atoi(line+6);
        next = next_word(line); ret.sockets[i-1] = atoi(next);
        }
      }
    }
  if (ret.rarity < 0) ret.flags |= OF_COUNTED;
  if (ret.level < 0) ret.flags |= OF_IDENT;
  return ret;
  }

NPC_REC load_npc() {
  NPC_REC ret;
  byte flag;
  char line[1024];
  char *next;
  memset(&ret, 0, sizeof(NPC_REC));
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Speed ")) { next = next_word(line); ret.speed = atoi(next); }
      if (match(line, "Number ")) { next = next_word(line); ret.number = atoi(next); }
      if (match(line, "Hp ")) { next = next_word(line); ret.hp = atoi(next); }
      if (match(line, "MaxHp ")) { next = next_word(line); ret.max_hp = atoi(next); }
      if (match(line, "Mana ")) { next = next_word(line); ret.mana = atoi(next); }
      if (match(line, "Level ")) { next = next_word(line); ret.level = atoi(next); }
      if (match(line, "Attack ")) { next = next_word(line); ret.attack = atoi(next); }
      if (match(line, "Defense ")) { next = next_word(line); ret.defense = atoi(next); }
      if (match(line, "Exp ")) { next = next_word(line); ret.exp = atof(next); }
      if (match(line, "ExpHp ")) { next = next_word(line); ret.exp_hp = atof(next); }
      if (match(line, "Special ")) { next = next_word(line); ret.special = atoui(next); }
      if (match(line, "Special2 ")) { next = next_word(line); ret.special2 = atoui(next); }
      if (match(line, "Sprite ")) { next = next_word(line); ret.sprite = atoi(next); }
      if (match(line, "MType ")) { next = next_word(line); ret.mtype = atoi(next); }
      if (match(line, "Name ")) { next = next_word(line); strcpy(ret.name, next); }
      if (match(line, "Time ")) { next = next_word(line); ret.time = atoi(next); }
      if (match(line, "Sleep ")) { next = next_word(line); ret.sleep = atoi(next); }
      if (match(line, "Fear ")) { next = next_word(line); ret.fear = atoi(next); }
      if (match(line, "Blind ")) { next = next_word(line); ret.blind = atoi(next); }
      if (match(line, "Confused ")) { next = next_word(line); ret.confused = atoi(next); }
      if (match(line, "Poison ")) { next = next_word(line); ret.poison = atoi(next); }
      if (match(line, "Prev ")) { next = next_word(line); ret.prev = atoi(next); }
      if (match(line, "Treasure ")) { next = next_word(line); ret.treasure = atoi(next); }
      }
    }
  return ret;
  }

void load_known() {
  Int32 i;
  Int32 x,y;
  char line[1024];
  byte flag;
  for (x=0; x<=100; x++)
    for (y=0; y<=100; y++)
      known[x][y] = 0;
  x = 0;
  y = 0;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      for (i=0; i<strlen(line); i++) {
        if (line[i] == '1') if (x<101 && y<101) known[x++][y] = 1;
        if (line[i] == '0') if (x<101 && y<101) known[x++][y] = 0;
        if (x > 100) {
          x = 0;
          y++;
          }
        }
      }
    }
  }

void load_map() {
  Int32 x,y;
  char* next;
  char line[1024];
  byte flag;
  for (x=0; x<=100; x++)
    for (y=0; y<=100; y++)
      map[x][y] = 21;
  x = 0;
  y = 0;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      next = line;
      while (*next != 0) {
        if ((next[0] >= '0' && next[0] <= '9') || next[0] == '-') {
          if (x<101 && y<101) map[x++][y] = atoi(next);
          }
        next = next_word(next);
        if (x > 100) {
          x = 0;
          y++;
          }
        }
      }
    }
  }

HOUSE load_house() {
  HOUSE ret;
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  memset(&ret, 0, sizeof(HOUSE));
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Region ")) { next = next_word(line); ret.region = atoi(next); }
      if (match(line, "RegionX ")) { next = next_word(line); ret.rx = atoi(next); }
      if (match(line, "RegionY ")) { next = next_word(line); ret.ry = atoi(next); }
      if (match(line, "CityX ")) { next = next_word(line); ret.cx = atoi(next); }
      if (match(line, "CityY ")) { next = next_word(line); ret.cy = atoi(next); }
      if (match(line, "Mortgage ")) { next = next_word(line); ret.mortgage = atof(next); }
      if (match(line, "Payment ")) { next = next_word(line); ret.payment = atof(next); }
      if (match(line, "Interest ")) { next = next_word(line); ret.interest = atof(next); }
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        ret.inventory[i] = load_object();
        }
      }
    }
  return ret;
  }

void load_inventory() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        player.invent[i] = load_object();
        }
      }
    }
  }

void load_pack() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        player.pack[i] = load_object();
        }
      }
    }
  }

void load_portable_hole() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        player.hole[i] = load_object();
        }
      }
    }
  }

void load_party() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        player.party[i] = load_npc();
        }
      }
    }
  }

void load_completed_quests() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Quest ")) {
        next = next_word(line);
        i = atoi(next);
        player.quests[i] = 'Y';
        }
      }
    }
  }

void load_fkey_spells() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Spell")) {
        i = atoi(line+5);
        next = next_word(line);
        strcpy(player.fspell[i], next);
        }
      }
    }
  }

void load_options() {
  byte flag;
  char line[1024];
  char *next;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      next = next_word(line);
      if (match(line, "DungeonScaling ")) dungeon_scaling = *next;
      if (match(line, "PartyCombat ")) party_combat_mode = *next;
      if (match(line, "AutoTarget ")) auto_target = *next;
      if (match(line, "AutoPickupGold ")) auto_pickup_gold = *next;
      if (match(line, "AutoPickupTorch ")) auto_pickup_torch = *next;
      if (match(line, "AutoOpenDoors ")) auto_open_doors = *next;
      if (match(line, "AutoSearchWalls ")) auto_bump_walls = *next;
      if (match(line, "DelayRanged ")) delay_ranged = atoi(next);
      if (match(line, "DelayHit ")) delay_hit = atoi(next);
      if (match(line, "DelayTraps ")) delay_trap = atoi(next);
      }
    }
  }

void load_stats() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  Int32  c;
  flag = 0xff;
  stats.num_trophies = 0;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Count")) {
        i = atoi(line+5);
        next = next_word(line);
        c = atoi(next);
        stats.counts[i] = c;
        }
      if (match(line, "Trophy ")) {
        next = next_word(line);
        stats.trophies[stats.num_trophies++] = atoi(next);
        }
      }
    }
  }

void load_spellbook() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  Int32  c;
  flag = 0xff;
  i = 1;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Spell ")) {
        next = next_word(line);
        c = atoi(next);
        spellbook[i++] = c;
        }
      }
    }
  }

void load_pawn_shop(Int32 shop) {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Slot ")) {
        next = next_word(line);
        i = atoi(next);
        player.pawn[shop][i] = load_object();
        }
      }
    }
  }

void load_current_level() {
  byte flag;
  byte gflag;
  char line[1024];
  char *next;
//  Int32  i;
  Int32  group;
  Int32  mon;
  Int32  loot;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Group ")) {
        next = next_word(line);
        group = atoi(next);
        gflag = 0xff;
        while (gflag) {
          read_line(line);
          if (eosf) { flag = 0; gflag = 0; }
          if (_strnicmp(line, "}",1) == 0) gflag = 0;
          if (gflag) {
            if (match(line, "Monster ")) {
              next = next_word(line);
              mon = atoi(next);
              player.monster[mon][group] = load_npc();
              }
            }
          }
        }
      else if (match(line, "Loot ")) {
        next = next_word(line);
        loot = atoi(next);
        player.map_objects[loot] = load_object();
        }
      else if (match(line, "Known ")) {
        load_known();
        memcpy(&(player.known), &known, sizeof(KNOW_TYPE));
        }
      else if (match(line, "Map ")) {
        load_map();
        memcpy(&(player.map), &map, sizeof(MAP_TYPE));
        }
      }
    }
  }

DUNGEON_LEVEL load_dungeon_level() {
  byte flag;
  byte gflag;
  char line[1024];
  char *next;
  DUNGEON_LEVEL ret;
  Int32 group;
  Int32 mon;
  Int32 loot;
  memset(&ret, 0, sizeof(DUNGEON_LEVEL));
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}", 1) == 0) flag = 0;
    if (flag) {
      if (match(line, "UpX")) { next = next_word(line); ret.upx = atoi(next); }
      if (match(line, "UpY")) { next = next_word(line); ret.upy = atoi(next); }
      if (match(line, "DownX")) { next = next_word(line); ret.downx = atoi(next); }
      if (match(line, "DownY")) { next = next_word(line); ret.downy = atoi(next); }
      if (match(line, "Cleared")) { next = next_word(line); ret.cleared = atoi(next); }
      if (match(line, "Known ")) {
        load_known();
        memcpy(&(ret.known), &known, sizeof(KNOW_TYPE));
        }
      if (match(line, "Map ")) {
        load_map();
        memcpy(&(ret.map), &map, sizeof(MAP_TYPE));
        }
      if (match(line, "Group ")) {
        next = next_word(line);
        group = atoi(next);
        gflag = 0xff;
        while (gflag) {
          read_line(line);
          if (eosf) { flag = 0; gflag = 0; }
          if (_strnicmp(line, "}", 1) == 0) gflag = 0;
          if (gflag) {
            if (match(line, "Monster ")) {
              next = next_word(line);
              mon = atoi(next);
              ret.monsters[mon][group] = load_npc();
              }
            }
          }
        }
      else if (match(line, "Loot ")) {
        next = next_word(line);
        loot = atoi(next);
        ret.objects[loot] = load_object();
        }
      }
    }
  return ret;
  }

void load_current_dungeon() {
  byte flag;
  char line[1024];
  char *next;
//  Int32  i;
  Int32  level;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "NumLevels ")) {
        next = next_word(line);
        current_dungeon.num_levels = atoi(next);
        }
      if (match(line, "Number ")) {
        next = next_word(line);
        current_dungeon.dungeon_number = atoi(next);
        }
      if (match(line, "TreasureX ")) {
        next = next_word(line);
        current_dungeon.treasurex = atoi(next);
        }
      if (match(line, "TreasureY ")) {
        next = next_word(line);
        current_dungeon.treasurey = atoi(next);
        }
      if (match(line, "Level ")) {
        next = next_word(line);
        level = atoi(next);
        current_dungeon.level[level] = load_dungeon_level();
        }
      }
    }
  }

void load_known_maps() {
  byte flag;
  byte gflag;
  char line[1024];
  char *next;
//  Int32  i;
  Int32  map;
//  Int32  mon;
//  Int32  loot;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Map ")) {
        next = next_word(line);
        map = atoi(next);
        gflag = 0xff;
        while (gflag) {
          read_line(line);
          if (eosf) { flag = 0; gflag = 0; }
          if (_strnicmp(line, "}",1) == 0) gflag = 0;
          if (gflag) {
            if (match(line, "Known ")) {
              load_known();
              memcpy(&(known_maps[map]), &known, sizeof(KNOW_TYPE));
              }
            }
          }
        }
      }
    }
  }

void load_gear_manager() {
  byte flag;
  byte flag2;
  char line[1024];
  Int32  set;
  char *next;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}", 1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Set ")) {
        next = next_word(line);
        set = atoi(next);
        flag2 = 0xff;
        while (flag2) {
          read_line(line);
          if (eosf) { flag = 0; flag2 = 0; }
          if (_strnicmp(line, "}", 1) == 0) flag2 = 0;
          if (flag2) {
            if (match(line, "MainHand ")) strcpy(gear_manager[set].main_hand, next_word(line));
            if (match(line, "OffHand ")) strcpy(gear_manager[set].off_hand, next_word(line));
            if (match(line, "Neck1 ")) strcpy(gear_manager[set].neck_1, next_word(line));
            if (match(line, "Neck2 ")) strcpy(gear_manager[set].neck_2, next_word(line));
            if (match(line, "Shield ")) strcpy(gear_manager[set].shield, next_word(line));
            if (match(line, "Armor ")) strcpy(gear_manager[set].armor, next_word(line));
            if (match(line, "Cloak ")) strcpy(gear_manager[set].cloak, next_word(line));
            if (match(line, "Boots ")) strcpy(gear_manager[set].boots, next_word(line));
            if (match(line, "Helm ")) strcpy(gear_manager[set].helm, next_word(line));
            if (match(line, "RingL1 ")) strcpy(gear_manager[set].ring_l1, next_word(line));
            if (match(line, "RingL2 ")) strcpy(gear_manager[set].ring_l2, next_word(line));
            if (match(line, "RingR1 ")) strcpy(gear_manager[set].ring_r1, next_word(line));
            if (match(line, "RingR2 ")) strcpy(gear_manager[set].ring_r2, next_word(line));
            }
          }
        }
      }
    }
  }

void load_player() {
  byte flag;
  char line[1024];
  char *next;
  Int32  i;
  flag = 0xff;
  while (flag) {
    read_line(line);
    if (eosf) flag = 0;
    if (_strnicmp(line, "}",1) == 0) flag = 0;
    if (flag) {
      if (match(line, "Name ")) { next = next_word(line); strcpy(player.name, next); }
      if (match(line, "Age ")) { next = next_word(line); player.age = atoi(next); }
      if (match(line, "Sex ")) { next = next_word(line); player.sex = *next; }
      if (match(line, "Level ")) { next = next_word(line); player.level = atoi(next); }
      if (match(line, "Exp ")) { next = next_word(line); player.exp = atof(next); }
      if (match(line, "ExpNext ")) { next = next_word(line); player.exp_next = atof(next); }
      if (match(line, "Class ")) { next = next_word(line); player.class = atoi(next); }
      if (match(line, "Title ")) { next = next_word(line); strcpy(player.title, next); }
      if (match(line, "Race ")) { next = next_word(line); player.race = atoi(next); }
      if (match(line, "Mhp ")) { next = next_word(line); player.mhp = atoi(next); }
      if (match(line, "Chp ")) { next = next_word(line); player.chp = atoi(next); }
      if (match(line, "Health ")) { next = next_word(line); player.health = atoi(next); }
      if (match(line, "MMana ")) { next = next_word(line); player.mmana = atoi(next); }
      if (match(line, "CMana ")) { next = next_word(line); player.cmana = atoi(next); }
      if (match(line, "X ")) { next = next_word(line); player.x = atoi(next); }
      if (match(line, "Y ")) { next = next_word(line); player.y = atoi(next); }
      if (match(line, "MapX ")) { next = next_word(line); player.map_x = atoi(next); }
      if (match(line, "MapY ")) { next = next_word(line); player.map_y = atoi(next); }
      if (match(line, "Region ")) { next = next_word(line); player.region = atoi(next); }
      if (match(line, "Torch ")) { next = next_word(line); player.torch = atoi(next); }
      if (match(line, "Food ")) { next = next_word(line); player.food = atoi(next); }
      if (match(line, "Armor ")) { next = next_word(line); player.armour = atoi(next); }
      if (match(line, "Weapon ")) { next = next_word(line); player.weapon = atoi(next); }
      if (match(line, "Mount ")) { next = next_word(line); player.mount = atoi(next); }
      if (match(line, "Peg ")) { next = next_word(line); player.peg = atoi(next); }
      if (match(line, "Gold ")) { next = next_word(line); player.gold_coins = atoi(next); }
      if (match(line, "Mithril ")) { next = next_word(line); player.mithril_coins = atoi(next); }
      if (match(line, "Adamantite ")) { next = next_word(line); player.adamantite_coins = atoi(next); }
      if (match(line, "Bank ")) { next = next_word(line); player.bank = atoi(next); }
      if (match(line, "Interest ")) { next = next_word(line); player.interest = atoi(next); }
      if (match(line, "DLevel ")) { next = next_word(line); player.d_level = atoi(next); }
      if (match(line, "Diff ")) { next = next_word(line); player.diff = atoi(next); }
      if (match(line, "LandX ")) { next = next_word(line); player.land_x = atoi(next); }
      if (match(line, "LandY ")) { next = next_word(line); player.land_y = atoi(next); }
      if (match(line, "DName ")) { next = next_word(line); strcpy(player.d_name, next); }
      if (match(line, "Day ")) { next = next_word(line); player.day = atoi(next); }
      if (match(line, "Month ")) { next = next_word(line); player.month = atoi(next); }
      if (match(line, "Min ")) { next = next_word(line); player.min = atoi(next); }
      if (match(line, "Hour ")) { next = next_word(line); player.hour = atoi(next); }
      if (match(line, "Align ")) { next = next_word(line); player.align = atoi(next); }
      if (match(line, "InCity ")) { next = next_word(line); player.in_city = atoi(next); }
      if (match(line, "City ")) { next = next_word(line); player.city = atoi(next); }
      if (match(line, "HpCount ")) { next = next_word(line); player.hp_count = atoi(next); }
      if (match(line, "ManaCount ")) { next = next_word(line); player.mana_count = atoi(next); }
      if (match(line, "Phase ")) { next = next_word(line); player.phase = atoi(next); }
      if (match(line, "ShipX ")) { next = next_word(line); player.ship_x = atoi(next); }
      if (match(line, "ShipY ")) { next = next_word(line); player.ship_y = atoi(next); }
      if (match(line, "ShipR ")) { next = next_word(line); player.ship_r = atoi(next); }
      if (match(line, "InShip ")) { next = next_word(line); player.inship = atoi(next); }
      if (match(line, "Gateway ")) { next = next_word(line); player.gateway = atoi(next); }
      if (match(line, "CavPaid ")) { next = next_word(line); player.cav_paid = atoi(next); }
      if (match(line, "DungeonNum ")) { next = next_word(line); player.dungeon_number = atoi(next); }
      if (match(line, "TimePlayed ")) { next = next_word(line); player.time_played = atoi(next); }
      if (match(line, "Stat")) { 
        i = atoi(line+4);
        next = next_word(line); player.stat[i] = atoi(next);
        }
      if (match(line, "Ability")) { 
        i = atoi(line+7);
        next = next_word(line); player.ability[i] = atoi(next);
        }
      if (match(line, "Skill")) { 
        i = atoi(line+5);
        next = next_word(line); player.skill[i] = atoi(next);
        }
      if (match(line, "Effect")) { 
        i = atoi(line+6);
        next = next_word(line); player.effect[i] = atoi(next);
        }
      if (match(line, "Gen")) { 
        i = atoi(line+3);
        next = next_word(line); player.gen[i] = atoi(next);
        }
      if (match(line, "Lore")) { 
        i = atoi(line+4);
        next = next_word(line); player.lore[i] = atoi(next);
        }
      }
    }
  }

void load() {
  Int32 change_set;
  char line[1024];
  char filename[sizeof(backupname)];
//  char ver[40];

  change_set = 0;
  strcpy(filename,loadname);
  strcat(filename,".sav");
  strcpy(backupname,loadname);
  strcat(backupname,".bak");
  save_file = fopen(filename,"rb");
  if (save_file == NULL) {
    printf("Fatal Error: Could not open %s\n",filename);
    exit(1);
    }
  dbuffer_pos = 0;
  dbuffer_count = 0;
  eosf = 0;
  encrypted = 0;
  read_line(buffer);
  if (buffer[1] == '6' || buffer[1] == '5') {
    printf("Save file version %s is no longer supported by the native loader\n",buffer);
    fclose(save_file);
    exit(1);
    }
  if (buffer[1] == '4' || buffer[1] == '3' || buffer[1] == '2' || buffer[1] == '1') {
    printf("save file version: %s\n",buffer);
    printf("Where did you get such an ancient save file?  I am sorry but there is\n");
    printf("no support for save versions older than 5.2\n");
    fclose(save_file);
    exit(1);
    }
  change_set = -1;
  if (strcmp(buffer, "v7.0.0") == 0) { change_set = 0; encrypted = 0; }
  if (strcmp(buffer, "v7.0.0e") == 0) { change_set = 0; encrypted = -1; }
  if (change_set < 0) {
    printf("Save file does not appear to be correct.  Cannot load\n");
    fclose(save_file);
    exit(1);
    }
  offset = 1;
  while (!eosf) {
    read_line(line);
    if (match(line, "Player {")) load_player();
    if (match(line, "Options {")) load_options();
    if (match(line, "Inventory {")) load_inventory();
    if (match(line, "Pack {")) load_pack();
    if (match(line, "PortableHole {")) load_portable_hole();
    if (match(line, "Party {")) load_party();
    if (match(line, "CompletedQuests {")) load_completed_quests();
    if (match(line, "FKeySpells {")) load_fkey_spells();
    if (match(line, "House ")) player.houses[atoi(next_word(line))] = load_house();
    if (match(line, "PawnShop ")) load_pawn_shop(atoi(next_word(line)));
    if (match(line, "CurrentLevel {")) load_current_level();
    if (match(line, "KnownMaps {")) load_known_maps();
    if (match(line, "CurrentDungeon {")) load_current_dungeon();
    if (match(line, "Stats {")) load_stats();
    if (match(line, "Spellbook {")) load_spellbook();
    if (match(line, "GearManager {")) load_gear_manager();
    }
  fclose(save_file);
  read_character_rules();
#ifdef HC
  if (player.effect[PE_HEROIC] >= 0) {
    remove(backupname);
    rename(filename, backupname);
    }
#endif
  }


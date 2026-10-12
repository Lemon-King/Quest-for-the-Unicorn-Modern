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

void clear_game_data() {
  Int32 i;
  Int32 x,y;
/* ***************************************** */
/* ***** Clear data from player record ***** */
/* ***************************************** */
  strcpy(player.name, "");
  player.age = 0;
  player.sex = 0;
  player.level = 0;
  player.exp = 0;
  player.exp_next = 0;
  player.class = 0;
  strcpy(player.title, "");
  player.race = 0;
  player.mhp = 0;
  player.chp = 0;
  player.health = 0;
  player.mmana = 0;
  player.cmana = 0;
  player.x = 0;
  player.y = 0;
  player.map_x = 0;
  player.map_y = 0;
  player.region = 0;
  player.torch = 0;
  player.food = 0;
  player.armour = 0;
  player.weapon = 0;
  player.mount = 0;
  player.peg = 0;
  player.gold_coins = 0;
  player.mithril_coins = 0;
  player.adamantite_coins = 0;
  player.bank = 0;
  player.interest = 0;
  player.d_level = 0;
  player.diff = 0;
  player.land_x = 0;
  player.land_y = 0;
  strcpy(player.d_name, "");
  player.day = 0;
  player.month = 0;
  player.min = 0;
  player.hour = 0;
  player.align = 0;
  player.in_city = 0;
  player.city = 0;
  player.hp_count = 0;
  player.mana_count = 0;
  player.phase = 0;
  player.ship_x = 0;
  player.ship_y = 0;
  player.ship_r = 0;
  player.inship = 0;
  player.gateway = 0;
  player.cav_paid = 0;
  player.dungeon_number = 0;
  player.time_played = 0;
  for (i=0; i<7; i++) player.stat[i] = 0;
  for (i=0; i<11; i++) player.ability[i] = 0;
  for (x=0; x<=100; x++)
    for (y=0; y<=100; y++) {
      player.map[x][y] = 0;
      player.known[x][y] = 0;
      }
  for (i=0; i<31; i++) player.skill[i] = 0;
  for (i=0; i<20; i++) player.invent[i].otype = O_NONE;
  for (i=0; i<33; i++) player.pack[i].otype = O_NONE;
  for (i=0; i<255; i++) player.effect[i] = 0;
  for (i=0; i<6; i++) player.party[i].hp = 0;
  for (i=0; i<5000; i++) player.gen[i] = 0;
  for (i=0; i<5000; i++) player.lore[i] = 0;
  for (i=0; i<13; i++) strcpy(player.fspell[i], "");
  for (i=0; i<11; i++) {
    player.tardis_x[i] = 0;
    player.tardis_y[i] = 0;
    player.tardis_r[i] = 0;
    strcpy(player.tardis_d[i], "");
    }
  for (i=0; i<1000; i++) player.hole[i].otype = O_NONE;
  for (i=0; i<1000; i++) player.quests[i] = 0;
  for (x=0; x<6; x++)
    for (y=0; y<61; y++)
      player.monster[x][y].hp = 0;
  for (i=0; i<1000; i++) player.map_objects[i].otype = O_NONE;
  for (x=0; x<10; x++)
    for (y=0; y<11; y++)
      player.pawn[x][y].otype = O_NONE;
  for (i=0; i<5; i++) {
    player.houses[i].region = 0;
    player.houses[i].rx = 0;
    player.houses[i].ry = 0;
    player.houses[i].cx = 0;
    player.houses[i].cy = 0;
    player.houses[i].mortgage = 0;
    player.houses[i].payment = 0;
    player.houses[i].interest = 0;
    for (x=0; x<500; x++) player.houses[i].inventory[x].otype = O_NONE;
    }

  /* **************************** */
  /* ***** Clear known maps ***** */
  /* **************************** */
  for (i=0; i<NO_OF_MAPS; i++)
    memcpy(&(known_maps[i]), &(player.known), sizeof(KNOW_TYPE));

  /* ****************************** */
  /* ***** Clear stats record ***** */
  /* ****************************** */
  for (i=0; i<255; i++) {
    stats.counts[i] = 0;
    stats.trophies[i] = 0;
    }
  stats.num_trophies = 0;

  /* **************************** */
  /* ***** Clear game flags ***** */
  /* **************************** */
  in_arena = 'N';
  wizardmode = 'N';
  used_sunstone = 'N';
  last_target_x = 0;
  last_target_y = 0;
  score1 = 0;
  score2 = 0;

  /* ************************** */
  /* ***** Clear messages ***** */
  /* ************************** */
  for (i=0; i<MAX_MSG; i++) {
    strcpy(messages[i],"");
    msg_clr[i] = 15;
    msg_name_clr[i] = 0;
    }

  /* **************************** */
  /* ***** Clear spell book ***** */
  /* **************************** */
  for (i=0; i<1000; i++) spellbook[i] = 0;

  /* ****************************** */
  /* ***** Clear gear manager ***** */
  /* ****************************** */
  for (i=0; i<10; i++) {
    strcpy(gear_manager[i].main_hand, "");
    strcpy(gear_manager[i].off_hand, "");
    strcpy(gear_manager[i].neck_1, "");
    strcpy(gear_manager[i].neck_2, "");
    strcpy(gear_manager[i].shield, "");
    strcpy(gear_manager[i].armor, "");
    strcpy(gear_manager[i].ring_l1, "");
    strcpy(gear_manager[i].ring_l2, "");
    strcpy(gear_manager[i].ring_r1, "");
    strcpy(gear_manager[i].ring_r2, "");
    strcpy(gear_manager[i].cloak, "");
    strcpy(gear_manager[i].boots, "");
    strcpy(gear_manager[i].helm, "");
    }

  /* ********************************* */
  /* ***** Clear current dungeon ***** */
  /* ********************************* */
  current_dungeon.num_levels = 0;
  current_dungeon.dungeon_number = 0;
  current_dungeon.treasurex = 0;
  current_dungeon.treasurey = 0;
  for (i=0; i<10; i++) {
    current_dungeon.level[i].upx = 0;
    current_dungeon.level[i].upy = 0;
    current_dungeon.level[i].downx = 0;
    current_dungeon.level[i].downy = 0;
    current_dungeon.level[i].cleared = 0;
    for (x=0; x<=100; x++)
      for (y=0; y<=100; y++) {
        current_dungeon.level[i].map[x][y] = 0;
        current_dungeon.level[i].known[x][y] = 0;
        }
    for (x=0; x<1000; x++)
      current_dungeon.level[i].objects[x].otype = O_NONE;
    for (x=0; x<6; x++)
      for (y=0; y<61; y++)
        current_dungeon.level[i].monsters[x][y].hp = 0;
    }
  }


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

static Int32 light_distance(void) {
  Int32 see_dist;
  see_dist = 1;
  if (player.hour >=5 && player.hour <6) see_dist = 2;
  if (player.hour >=6 && player.hour < 7) see_dist = 3;
  if (player.hour >=7 && player.hour < 19) see_dist = 5;
  if (player.hour >=19 && player.hour < 20) see_dist = 3;
  if (player.hour >=20 && player.hour < 21) see_dist = 2;
  return see_dist;
  }

static void land_discover(void) {
  Int32 i,j,color = 0;
  Int32 tile;
  Int32 see_dist;
  see_dist = light_distance();
  for (i=player.x-see_dist; i<=player.x+see_dist; i++)
    for (j=player.y-see_dist; j<=player.y+see_dist; j++)
      if (reveal_land(i,j)) {
        tile = square_value(i,j);
        if (i-player.map_x >= 0 && i-player.map_x <= TILES_X &&
            j-player.map_y >= 0 && j-player.map_y <= TILES_Y)
          show_sprite(i-player.map_x,j-player.map_y,tile);

        /* Without the Seamless World Map preference a tile with no colour of its own keeps the one before it */
        if (land_color(tile) != 0 || seamless_world()) color = land_color(tile);
        plot_minimap(i,j,color);
        }
  close_display();
  } /* void land_discover() */

/* The Gateway's entrance, in its region while it shows */
static void place_gateway(void) {
  if (player.region != GATEWAY_REGION || gateway_tile() == 0) return;

  player.map[GATEWAY_X][GATEWAY_Y] = gateway_tile();
  }

/* The player steps into region: the squares known in the one they leave are kept, and the new one's map comes in */
static void enter_region(Int32 region) {
  memcpy(&(known_maps[player.region]), &(player.known), sizeof(KNOW_TYPE));
  player.region = region;
  memcpy(&(player.known), &(known_maps[player.region]), sizeof(KNOW_TYPE));
  copyMap(&player.map,player.region);
  }

/* The view follows the player out of region: with the Seamless World Map preference over the same part of the world,
   or else to the new region's corner map_x,map_y, or centred on the player with the follow camera */
static void follow_view(Int32 region,Int32 map_x,Int32 map_y) {
  if (seamless_world()) {
    view_from_region(region);
    adjust_map();
    return;
    }

  player.map_x = map_x;
  player.map_y = map_y;
  adjust_map();
  camera_arrive();
  }

static void check_region(void) {
  char  flag;
  Int32 from;
  flag = ' ';
  if (player.y == 0 && player.region>4) {
    from = player.region;
    enter_region(player.region-4);
    player.y = 100;
    follow_view(from,player.x-8,player.y-15);
    flag = '*';
    }
  if (player.y == 101 && player.region<9) {
    from = player.region;
    enter_region(player.region+4);
    player.y = 1;
    follow_view(from,player.x-8,player.y-8);
    flag = '*';
    }
  if (player.x == 101 && player.region != 4 && player.region != 8 &&
      player.region != 12) {
    from = player.region;
    enter_region(player.region+1);
    player.x = 1;
    follow_view(from,player.x-8,player.y-8);
    flag = '*';
    }
  if (player.x == 0 && player.region != 1 && player.region != 5 &&
      player.region != 9) {
    from = player.region;
    enter_region(player.region-1);
    player.x = 100;
    follow_view(from,player.x-15,player.y-8);
    flag = '*';
    }
  strcpy(player.d_name,regions[player.region-1].name);
  place_gateway();
  if (player.inship == ' ' && player.ship_r == player.region) {
    player.map[player.ship_x][player.ship_y] = 12;
    }
  if (player.x<=1) player.x = 1;
  if (player.x>100) player.x = 100;
  if (player.y<=1) player.y = 1;
  if (player.y>100) player.y = 100;

  /* The joined overworld already shows the new region, but the region's name on the frame changes, and the view may
     have scrolled with the player; move_land() draws the player and the minimap */
  if (flag != ' ' && seamless_world()) show_land();
  else if (flag != ' ')
    { show_land(); show_sm_land(); show_land_player(); }
  } /* void check_region() */

static char valid(Int32 x,Int32 y) {
  char flag;
  flag = '*';
  if (player.inship == ' ') {
    if (player.map[x][y] == 3) flag = ' ';
    if (player.map[x][y] == 4) flag = ' ';
    if (player.peg != 0) flag = '*';
    }
  else {
    flag = ' ';
    if (player.map[x][y] ==4) {
      flag = '*';
      add_skill(SK_BOAT,1);
      }
    }
  if (flag != '*' && player.map[x][y] == 4 && has_intrinsic(A_BOAT)) {
    flag = '*';
    add_skill(SK_BOAT,1);
    }
  if (has_intrinsic(A_WATER) && player.map[x][y] == 4) flag = '*';
  if (has_intrinsic(A_FLY)) flag = '*';
  return flag;
  } /* char valid(Int32 x,Int32 y) */

/* valid() for the player on foot, while in a ship */
static char valid_on_foot(Int32 x,Int32 y) {
  char inship;
  char flag;

  inship = player.inship;
  player.inship = ' ';
  flag = valid(x,y);
  player.inship = inship;

  return flag;
  }

/*
void adjust_player_map() {
  char changed;
  changed = 'N';
  if (player.y - player.map_y < 2) {
    player.map_y = player.y - (TILES_Y-4);
    changed = 'Y';
    if (player.map_y < 1) player.map_y = 1;
    }
  if (player.y - player.map_y > TILES_Y-2) {
    player.map_y = player.y - 4;
    changed = 'Y';
    if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
    }
  if (player.x - player.map_x < 2) {
    player.map_x = player.x - TILES_X-4;
    changed = 'Y';
    if (player.map_x < 1) player.map_x = 1;
    }
  if (player.x - player.map_x > TILES_X-2) {
    player.map_x = player.x - 4;
    changed = 'Y';
    if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
    }
  if (changed == 'Y') show_land();
  }
*/

/* Steps by dx,dy, into the next region at its edge. Returns 'N' when the step was blocked. */
static char move_land(Int32 dx,Int32 dy) {
  Int32 old_x,old_y;
  Int32 old_region;
  Int32 blocked;
  Int32 shore;
  char  moved;

  old_x = player.x;
  old_y = player.y;
  old_region = player.region;

  remove_land_player();
  player.x = player.x+dx;
  player.y = player.y+dy;
  check_region();

  /* With Ship Improvements a ship that runs into walkable shore twice puts the player ashore */
  blocked = (valid(player.x,player.y) == ' ' && !sails_into_town(player.x,player.y));
  shore = (blocked && player.inship != ' ' && player.region == old_region && valid_on_foot(player.x,player.y) == '*');
  if (steps_ashore(old_x,old_y,dx,dy,shore)) {
    blocked = 0;
    show_spot(old_x,old_y);
    }
  if (blocked) {
    player.x = player.x-dx;
    player.y = player.y-dy;
    }
  else walk_aboard();
  if (adjust_map() == 'Y') show_land();
  if (seamless_world() && minimap_moved()) show_sm_land();
  show_land_player();

  moved = (player.x != old_x || player.y != old_y || player.region != old_region) ? 'Y' : 'N';
  if (moved == 'Y' || change_free_upland_keys != 'Y') stats.counts[STAT_STEPS_TAKEN]++;

  return moved;
  } /* char move_land(Int32 dx,Int32 dy) */

static void enter_dungeon(void) {
  char flag;
  Int32  x;
  LINK_REC link;
  if (player.map[player.x][player.y] == 31) {
    exitf = 2;
    player.in_city = 'N';
    player.d_level = 0;
    set_gateway_dungeon();
    build_dungeon(7,player.dungeon_number);
    }
  if (player.map[player.x][player.y] == 17) {
    exitf = 5;
    player.d_level = 0;
    strcpy(player.d_name,"Village");
    player.in_city = 'Y';
    player.city = 15;
    player.dungeon_number = -1;
    }
  if (player.map[player.x][player.y] == 5) {
    x = 0;
    link = links[x++];
    flag = '*';
    if (link.x != player.x || link.y != player.y || link.typ != 5 ||
        link.region != player.region) flag = ' ';
    while (flag == ' ') {
      link = links[x++];
      flag = '*';
      if (link.x != player.x || link.y != player.y || link.typ != 5 ||
          link.region != player.region) flag = ' ';
      if (link.region == 99) flag = '*';
      }
    if (link.region != 99) {
      exitf = 5;
      player.d_level = 0;
      strcpy(player.d_name,link.name);
      player.in_city = 'Y';
      player.city = link.diff;
      player.dungeon_number = x-1;

      /* Without the Town Completion change nothing marks a town complete */
      if (change_town_completion == 'Y') player.quests[player.dungeon_number] = 'Y';
      }
    }
  if (player.map[player.x][player.y] == 11) {
    x = 0;
    link = links[x++];
    flag = '*';
    if (link.x != player.x || link_y(x-1) != player.y || link.typ != 11 ||
        link.region != player.region) flag = ' ';
    while (flag == ' ') {
      link = links[x++];
      flag = '*';
      if (link.x != player.x || link_y(x-1) != player.y || link.typ != 11 ||
          link.region != player.region) flag = ' ';
      if (link.region == 99) flag = '*';
      }
    if (link.region != 99) {
      exitf = 2;
      player.in_city = 'N';
      player.d_level = 0;
      player.diff = link.diff;
      strcpy(player.d_name,link.name);
      player.dungeon_number = x-1;
      build_dungeon(7,player.dungeon_number);
      }
    }
  } /* void enter_dungeon() */

/*
void eat() {
  OBJECT_REC* item;
  inv_item = 0;
  item = NULL;
  if (player.health > 100) {
    clear_prompt();
    prompt("Too full to eat any more");
    return;
    }
  if (item == NULL) {
    saveScreen();
    item = inventory(NULL);
    restoreScreen();
    }
  if (item != NULL && item->otype == O_FOOD) {
    if (item->charges != -1) item->charges--;
    if (item->charges == 0) {
      item->otype = O_NONE;
      }
    perform_effect(item);
    }
  show_spot(player.x,player.y);
  show_player();
  show_side();
  }
*/

/* Returns 'Y' when an item was used, or the inventory screen moved one */
static char land_use_item(void) {
  OBJECT_REC* item;
  char        acted;

  item = NULL;
  saveScreen();
  item = inventory(item);
  acted = takes_time;
  restoreScreen();
  show_spot(player.x,player.y);
  show_player();
  if (item != NULL && item->otype != O_NONE) {
    acted = 'Y';
    if (item->otype == O_FOOD && player.health > 100) {
      msg("Too full to eat any more",10);
      }
    else {
      if (item->charges != -1) item->charges--;
      if (item->charges == 0) item->otype = O_NONE;
      perform_effect(item);
      }
    }
  graphmode();
  show_map();
  show_sm_map();
  land_discover();
  show_side();
  show_player();
  show_messages();
  clair = ' ';

  return acted;
  } /* char land_use_item() */

static void hunt(void) {
  Int32 chance;
  Int32 item_count;
  Int32 item;
  Int32 i;
  Int32 skill;
  chance = 10;
  switch (player.map[player.x][player.y]) {
    case 2:chance = 25; break;
    case 3:chance = 20; break;
    case 4:chance = 0; break;
    case 6:chance = 5; break;
    case 8:chance = 5; break;
    case 9:chance = 17; break;
    }

  /* Without the Hunting Skill change the chance grows with the Mace skill, though hunting trains Scouting */
  skill = (change_hunting_skill == 'Y') ? get_skill(SK_SCOUT) : get_skill(SK_MACE);
  if (skill>4 && skill<10) chance = chance*2;
  if (skill>9 && skill<15) chance = chance*3;
  if (skill>14) chance = chance*4;
  if (u_random(100)<chance) {
    item = -1;
    item_count = 0;
    while (hunting_items[item_count].item != 9999) item_count++;
    while (item < 0) {
      i = u_random(item_count);
      if (u_random(100000) <= hunting_items[i].percent) item = hunting_items[i].item;
      }
    inventory(find_linked_item(item));
    show_screen();
    add_skill(SK_SCOUT,1);
    }
  else {
    msg("Nothing found",14);
    }
  } /* void hunt() */

static void land_go_ship(void) {
  Int32 x,y;

  if (player.inship != ' ') {
    x = player.x;
    y = player.y;
    dock_square(&x,&y);
    player.inship = ' ';
    player.map[x][y] = 12;
    show_spot(x,y);
    player.ship_x = x;
    player.ship_y = y;
    player.ship_r = player.region;
    }
  else if (player.map[player.x][player.y] == 12) {
    player.inship = '*';
    player.map[player.x][player.y] = 4;
    }
  show_land_player();
  } /* void land_go_ship() */

/* Programs destination i with your square and the description typed; ESC, with Improved Interface, leaves it as it was */
static void new_tardis(Int32 i) {
  TEXT_FIELD field;
  STRING32   description;
  if (has_intrinsic(A_TARD)) {
    strcpy(description,"");
    open_log_field(&field,description,sizeof(description),"Desc ? ",10);
    field.cancel = (improved_interface == 'Y');
    if (!read_field(&field)) return;

    player.tardis_x[i] = player.x;
    player.tardis_y[i] = player.y;
    player.tardis_r[i] = player.region;
    strcpy(player.tardis_d[i],description);
    prompt("Tardis Programmed");
    }
  } /* void new_tardis(Int32 i) */

static void tardis(Int32 i) {
  if (has_intrinsic(A_TARD))
    if (player.tardis_r[i] != 0) {
      enter_region(player.tardis_r[i]);
      player.x = player.tardis_x[i];
      player.y = player.tardis_y[i];
      player.map_x = player.x-8;
      player.map_y = player.y-8;

      /* The joined overworld's view stops only at the world's edges, where adjust_map() keeps it */
      if (!seamless_world()) {
        if (player.map_x<1) player.map_x = 1;
        if (player.map_y<1) player.map_y = 1;
        if (player.map_x>86) player.map_x = 86;
        if (player.map_y>86) player.map_y = 86;
        }
      adjust_map();
      camera_arrive();
      if (player.inship == ' ' && player.ship_r == player.region) {
            player.map[player.ship_x][player.ship_y] = 12;
        }

      /* The joined overworld names the region it lands in; 7.0 kept the old name until the next region crossing */
      if (seamless_world()) strcpy(player.d_name,regions[player.region-1].name);
      place_gateway();
      show_map();
      show_sm_map();
      show_side();
      show_land_player();
      }
  } /* void tardis(Int32 i) */

static void tardis_log(void) {
  Int32 i;
  char buffer[80];
  textmode(3);
  textcolor(2); gotoxy(29,1);
  txt_writeln("Tardis Destination Log");
  gotoxy(23,3);
  txt_writeln("      Region   X   Y   Description");
  textcolor(14);
  for (i=1; i<=10; i++) {
    gotoxy(23,i+4);
    sprintf(buffer,"%2d      %2d    %2d  %2d   %s",i,
            player.tardis_r[i], player.tardis_x[i],
            player.tardis_y[i], player.tardis_d[i]);
    txt_write(buffer);
    }
  gotoxy(28,24); textcolor(12);
  txt_write("Press any key to continue");
  inkey();
  graphmode();
  show_map();
  show_sm_map();
  show_side();
  show_land_player();
  } /* void tardis_log() */

/* Returns 'Y' when the inventory screen moved an item */
static char do_inventory(void) {
  saveScreen();
  inventory(NULL);
  restoreScreen();

  return takes_time;
  } /* char do_inventory() */

static void gps(void) {
  char buffer[20];
  sprintf(buffer,"Region    : %d",player.region);
  msg(buffer,15);
  sprintf(buffer,"X,Y       : %d,%d",player.x,player.y);
  msg(buffer,15);
  }

/* The overworld's whole screen, after a screen that covered it */
static void redraw_land(void) {
  graphmode();
  show_land();
  show_sm_land();
  show_land_player();
  show_side();
  show_messages();
  }

/* An overworld turn: regeneration, the clock and food, and the chance of an encounter */
static void land_turn(void) {
  Int32 i;

  if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();

  player.hp_count = player.hp_count-1;
  for (i=1; i<=19; i++) {
    if (player.invent[i].otype != O_NONE) {
      if ((i==I_NCK1 || i==I_NCK2) &&
          player.invent[i].otype == O_NECK &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if ((i==I_RNG1 || i==I_RNG2 || i==I_RNG3 || i==I_RNG4) &&
          player.invent[i].otype == O_RING &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (i==I_SHLD &&
          player.invent[i].otype == O_SHLD &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (i==I_ARMR &&
          player.invent[i].otype == O_ARMR &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (i==I_ROBE &&
          player.invent[i].otype == O_ROBE &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (i==I_BOOT &&
          player.invent[i].otype == O_BOOT &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (i==I_HELM &&
          player.invent[i].otype == O_HELM &&
          player.invent[i].effect == E_INTR &&
          player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      else if (player.invent[i].otype != O_NECK &&
               player.invent[i].otype != O_RING &&
               player.invent[i].otype != O_SHLD &&
               player.invent[i].otype != O_ARMR &&
               player.invent[i].otype != O_ROBE &&
               player.invent[i].otype != O_BOOT &&
               player.invent[i].otype != O_HELM &&
               player.invent[i].effect == E_INTR &&
               player.invent[i].control == A_HEAL)
        player.hp_count -= player.invent[i].damage;
      }
    }

  if (player.hp_count<1) {
    player.hp_count = 2;
    if (player.chp<player.mhp) player.chp = player.chp+1;
    show_side();
    }

  player.mana_count = player.mana_count-1;
  if (player.mana_count<1) {
    player.mana_count = 3;
    if (player.cmana<player.mmana) player.cmana = player.cmana+1;
    show_side();
    }

  take_time();
  check_encounter();
  } /* void land_turn() */

void up_land() {
  byte key;
  Int32  x = 0,y = 0;
  Int32  i;
//  Int32  m_file;
  Int32  suggest;
  Int32  sug_level;
  char   turn;
  Int32  command;
  char   seamless;
  char   camera;

/* ******************************************* */
/* ***** Actual beginning of upland code ***** */
/* ******************************************* */
  play_music(MUSIC_UPLAND);
  forget_compass();
  while (keypressed()) key=inkey();
  in_upland = 'Y';
  player.d_level = 0;

  /* Without the Gateway Exit change the Gateway's rules stay on after leaving it */
  if (change_gateway_exit == 'Y' && player.gateway == '*') player.gateway = ' ';
  copyMap(&player.map,player.region);
//#ifdef MINGW
//  m_file = open("temp01.$$$",O_RDONLY | O_BINARY);
//#else
//  m_file = open("temp01.$$$",O_RDONLY);
//#endif
//  if (m_file < 0) {
//    printf("Fatal Error: Could not open temp01.$$$\n");
//    exit(1);
//    }
//  lseek(m_file,sizeof(KNOW_TYPE) * player.region,SEEK_SET);
//  read(m_file,&(player.known),sizeof(KNOW_TYPE));
//  close(m_file);
  memcpy(&(player.known), &(known_maps[player.region]), sizeof(KNOW_TYPE));
  player.x = player.land_x;
  player.y = player.land_y;
  ship_leaves_town();
  player.map_x = player.x-8;
  player.map_y = player.y-8;
  adjust_map();
  camera_arrive();
  track_ship();
  if (player.inship == ' ' && player.ship_r == player.region) {
      player.map[player.ship_x][player.ship_y] = 12;
    }
  strcpy(player.d_name,regions[player.region-1].name);
  place_gateway();
  graphmode();
  show_land();
  show_sm_land();
  show_side();
  land_discover();
  show_land_player();
  exitf = 0;
  while (exitf == 0) {
    show_time();

    /* turn is 'N' for a key that takes no time; a cancelled save_gear() or load_gear() clears takes_time */
    turn = 'Y';
    takes_time = 'Y';
    key = inkey();

    /* TAB opens the Tab Menu, which types the command chosen, or loads another game */
    if (key == 9 && improved_interface == 'Y') {
      command = open_tab_menu();
      if (command == TAB_LOAD_GAME) exitf = 11;
      if (command <= 0) continue;
      key = (byte)command;
      }

    switch (key) {
      /* Without the Upland Quit change Q quits at once, unasked, and falls through into a step north */
      case 'Q':if (change_upland_quit == 'Y') {
                 if (check_quit()) exitf = 3;
                 turn = 'N';
                 break;
                 }
               exitf = 3;
      case '8':turn = move_land(0,-1); break;
      case '2':turn = move_land(0,1); break;
      case '4':turn = move_land(-1,0); break;
      case '6':turn = move_land(1,0); break;
      case '7':turn = move_land(-1,-1); break;
      case '9':turn = move_land(1,-1); break;
      case '1':turn = move_land(-1,1); break;
      case '3':turn = move_land(1,1); break;
      case '>':enter_dungeon(); break;
      case '}':save_gear('N',-1);
               turn = takes_time;
               break;
      case ']':load_gear('N',-1);
               turn = takes_time;
               break;
      case '{':save_gear('Y',-1);
               turn = takes_time;
               break;
      case '[':load_gear('Y',-1);
               turn = takes_time;
               break;
      case 'e':eat(); break;
      case 'H':show_icons();
               turn = 'N';
               break;
      case 'M':if (world_map != 'N') {
                 show_world_map();
                 redraw_land();
                 }
               turn = 'N';
               break;
      case 'h':hunt(); break;
      case 's':hunt(); break;
      case 'i':turn = do_inventory(); break;
      case 'E':land_go_ship(); break;
      case '?':upland_help();
               turn = 'N';
               break;
      case 'S':
          player.land_x = player.x;
          player.land_y = player.y;
          save_new();
          hold_save(loadname);
          graphmode();
          show_land();
          show_sm_land();
          show_side();
          show_land_player();
          break;
      case 'u':turn = land_use_item(); break;
      case 'v':view(player.map[x][y]); break;
      case 'X':exitf = 4;
               turn = 'N';
               break;
      case 'R':
          disp_char();
          graphmode();
          show_land();
          show_sm_land();
          show_land_player();
          show_side();
          turn = 'N';
          break;
      case 'O':
          seamless = seamless_world_map;
          camera = follow_camera;
          game_options();

          /* Turning Seamless World Map off can leave the view past the region's edges, and the follow camera moves it */
          if (set_screen_layout() || seamless_world_map != seamless || follow_camera != camera) adjust_map();
          redraw_land();
          turn = 'N';
          break;
      case '~':gps();
               turn = 'N';
               break;
      case KY_F1:tardis(1); break;
      case KY_F2:tardis(2); break;
      case KY_F3:tardis(3); break;
      case KY_F4:tardis(4); break;
      case KY_F5:tardis(5); break;
      case KY_F6:tardis(6); break;
      case KY_F7:tardis(7); break;
      case KY_F8:tardis(8); break;
      case KY_F9:tardis(9); break;
      case KY_F10:tardis(10); break;
      case KY_S_F1:new_tardis(1); break;
      case KY_S_F2:new_tardis(2); break;
      case KY_S_F3:new_tardis(3); break;
      case KY_S_F4:new_tardis(4); break;
      case KY_S_F5:new_tardis(5); break;
      case KY_S_F6:new_tardis(6); break;
      case KY_S_F7:new_tardis(7); break;
      case KY_S_F8:new_tardis(8); break;
      case KY_S_F9:new_tardis(9); break;
      case KY_S_F10:new_tardis(10); break;
      case 'T':if (has_intrinsic(A_TARD)) tardis_log();
               turn = 'N';
               break;
      default:turn = 'N';
      }

    /* A ship sailed onto a town with a harbour enters it (Ship Improvements) */
    if (sails_into_town(player.x,player.y)) enter_dungeon();

    /* Without the Free Upland Keys change every key passes a turn */
    if (change_free_upland_keys != 'Y') turn = 'Y';

    land_discover();
    track_ship();
    if (turn == 'Y') land_turn();
    if (player.chp<1) exitf = 6;
    if (player.health<=0) exitf = 6;
    if (has_intrinsic(A_COMP)) {
      i = 0;
      suggest = 0;
      sug_level = 999;
      if (player.effect[PE_COMPASS] > 0) {
        while (i >= 0) {
          if (links[i].quest == player.effect[PE_COMPASS]) {
            suggest = i;
            sug_level = links[i].quest;
            }
          i++;
          if (links[i].region > 90) i = -1;
          }
        } else {
        while (i >= 0) {
          if (player.quests[i] != 'Y' && links[i].quest > 0) {
            if (links[i].quest < sug_level) {
              suggest = i;
              sug_level = links[i].quest;
              }
            }
          i++;
          if (links[i].region > 90) i = -1;
          }
        }
      if (sug_level != 999) {
        x = compass(suggest);
/*
        if (abs(player.x-links[suggest].x)<4 && player.y<links[suggest].y) x = 92;
        else if (abs(player.x-links[suggest].x)<4 && player.y>links[suggest].y) x = 91;
        else if (player.x<links[suggest].x && abs(player.y-links[suggest].y)<4) x = 93;
        else if (player.x>links[suggest].x && abs(player.y-links[suggest].y)<4) x = 94;
        else if (player.x<links[suggest].x && player.y<links[suggest].y) x = 97;
        else if (player.x>links[suggest].x && player.y<links[suggest].y) x = 98;
        else if (player.x<links[suggest].x && player.y>links[suggest].y) x = 95;
        else if (player.x>links[suggest].x && player.y>links[suggest].y) x = 96;
        else x = 10;
*/
        show_compass(x);
        } else show_compass(10);
      }
    if (turn == 'Y' && has_intrinsic(A_FOOD) && player.health < 100) player.health++;
    if (get_str() <= 0 || get_dex() <= 0 || get_int() <= 0 ||
        get_con() <= 0 || get_wis() <= 0 || get_chr() <= 0) exitf = 9;
    }
  if (exitf == 3) mod_exit = 3;
  if (exitf == 2) mod_exit = 2;
  if (exitf == 4) mod_exit = 4;
  if (exitf == 5) mod_exit = 5;
  if (exitf == 6) mod_exit = 6;
  if (exitf == 9) mod_exit = 9;
  if (exitf == 11) mod_exit = 8;
//#ifdef MINGW
//  m_file = open("temp01.$$$",O_WRONLY | O_BINARY);
//#else
//  m_file = open("temp01.$$$",O_WRONLY);
//#endif
//  if (m_file < 0) {
//    printf("Fatal Error: Could not open temp01.$$$\n");
//    exit(1);
//    }
//  lseek(m_file,sizeof(KNOW_TYPE) * player.region,SEEK_SET);
//  write(m_file,&(player.known),sizeof(KNOW_TYPE));
//  close(m_file);
  memcpy(&(known_maps[player.region]), &(player.known), sizeof(KNOW_TYPE));
  player.land_x = player.x;
  player.land_y = player.y;
  in_upland = 'N';
}


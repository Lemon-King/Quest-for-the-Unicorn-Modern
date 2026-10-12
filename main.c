/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#define MAIN

#include "header.h"

/* 1 when the game starts from a save: one named on the command line, or one picked from the title screen */
static Int32 loadgame;

/* A game picked from the title screen gets its <name>.cfg, as one named on the command line does. The window is
   already open, so the screen size stays. */
static void apply_character_config() {
  Int32 width;
  Int32 height;
  width = screen_width;
  height = screen_height;

  process_config(loadname);

  screen_width = width;
  screen_height = height;
  set_music_volume();
  if (music != 'Y') stop_music();
  }

static void start_game(Int32 loadgame) {
  clear_game_data();
  if (loadgame == 1) {
    textmode(3); gotoxy(1,1); textcolor(10);
    txt_write("Loading Game . . .");
    load();

    /* Play time counts from the load, as it does from make_char() */
    start_time = (Int32)time(NULL);
    if (player.d_level>0) mod_exit = 2;
      else mod_exit = 1;
    disp_char();
    }
  else {
    strcpy(loadname,"");
    strcpy(backupname,"");
    new_game();
    game_setup();
    make_char(0);
    mod_exit = 1;
    build_dungeon(7,0);

    /* clear_game_data() leaves dungeon 0 marked as built, so without the New Game Dungeon change its levels are empty */
    if (change_new_game_dungeon == 'Y') current_dungeon.dungeon_number = -1;
    }
  }

static void play_game() {
  exit_flag = ' ';
  while (exit_flag == ' ') {
    if (mod_exit == 1) up_land();
    if (mod_exit == 2) cave();
    if (mod_exit == 3) exit_flag = '*';
    if (mod_exit == 4) exit_flag = 'S';
    if (mod_exit == 5) cave();
    if (mod_exit == 6) exit_flag = 'D';
    if (mod_exit == 7) { make_char(1); mod_exit = 1; }
    if (mod_exit == 9) exit_flag = 'O';
    }
  }

static void end_game() {
  end_time = (Int32)time(NULL);
  player.time_played += (end_time - start_time);
  if (exit_flag == 'S') {
    save_new();
    }
  textmode(3);
  remove(backupname);
  score_position = 0;
  if (exit_flag == '*') {
    textcolor(12);
    gotoxy(21,12);
    txt_writeln("You have cowardly given up your quest!!");
    flush();
    delay(2500);
    final_score('Q');
    show_final_scores();
    }
  if (exit_flag == 'D') {
    textcolor(12);
    gotoxy(27,12);
    txt_writeln("Sorry but you have died!!");
    flush();
    delay(2500);
    final_score('D');
    show_final_scores();
    }
  if (exit_flag == 'O') {
    textcolor(12);
    gotoxy(22,12);
    txt_writeln("Sorry but you have died of old age!!");
    flush();
    delay(2500);
    final_score('D');
    show_final_scores();
    }
  check_scores();
  if (score1>0 || score2>0) show_scores();
  textmode(3);
  }

/* A failure the game cannot start after: the -s report prints why, and a player sees it in a message box */
static void fatal_error(char* message) {
  if (showStats) printf("Fatal Error: %s\n",message);
    else show_fatal_error(message);
  exit(1);
  }

/* Defaults, unicorn.cfg, the command line and <save>.cfg; the table counts, the maps and the -s report. Returns 0
   when the game should end without opening its window. */
Int32 configure_game(Int32 argc,char** argv,GAME_SETUP* setup) {
  Int32 i;
  time_t         date_time;
  Int32 loot_commons;
  Int32 loot_uncommons;
  Int32 loot_rares;
  Int32 loot_very_rares;
  Int32 loot_ultra_rares;
  char  error[256];

#ifdef DEBUG
  debug_tab = 0;
#endif
  strcpy(currentMacro,"");
  Rcs_Seed(1);
  current_dungeon.dungeon_number = -1;
  date_time = time(NULL);
  date = *(localtime(&date_time));
  date.tm_mon += 1;
  date.tm_year += 1900;
  screen_width = 640;
  screen_height = 480;
  tile_width = 25;
  tile_height = 27;
  TEXT_GRID_WIDTH = 80;
  TEXT_GRID_HEIGHT = 25;
  NUM_MSG = 10;
  for (i=0; i<12; i++) strcpy(macros[i],"");
  TILES_X = 16;
  TILES_Y = 16;
  fkey_mode = 'S';
  loot_commons = 0;
  loot_uncommons = 0;
  loot_rares = 0;
  loot_very_rares = 0;
  loot_ultra_rares = 0;
  showStats = 0;
  auto_target = 'Y';
  combat_mode = 's';
  soundon = 'Y';
  restock = 'N';
  loadgame = 0;
  bottom = 5;
  music = 'Y';
  music_volume = 100;
  game_thread = 'Y';
  integer_scaling = 'N';
  heroicMode = 0;
  skipcheck = 'N';
  in_upland = 'N';
  show_keys = 'N';
  auto_pickup_gold = 'N';
  auto_pickup_torch = 'N';
  auto_open_doors = 'N';
  auto_bump_walls = 'N';
  no_arena = 'Y';
  downgrade_arena = 'N';
  downgrade_depth = -1;
  dungeon_scaling = 'N';
  party_combat_mode = 'R';
  monster_count = 60;
  delay_ranged = 5;
  delay_hit = 200;
  delay_trap = 100;
  enhanced_sprites = 'N';
  floor_item_names = 'Y';
  gothic_scrolls = 'N';
  permadeath = 'Y';
  encrypt_save_file = 'Y';
  start_time = (Int32)time(NULL);
  remember_built_in_defaults();
  process_config("unicorn");
  for (i=1; i<argc; i++) {
    if (process_option(argv[i]) == 0) {
      loadgame = 1;
      strcpy(loadname,argv[i]);
      }
    }
  if (loadgame == 1) {
    for (i=0; i<strlen(loadname); i++)
      if (loadname[i] == '.') loadname[i] = 0;
    process_config(loadname);
    }
  remember_option_defaults();
  if (screen_width < 640) screen_width = 640;
  if (screen_height < 480) screen_height = 480;
  TILES_X = (screen_width - 240) / tile_width;
  TILES_Y = (screen_height - 48) / tile_height;
  screen_width = (TILES_X * tile_width) + 240;
  screen_height = (TILES_Y * tile_height) + 48;
  TEXT_GRID_WIDTH = screen_width / 8;
  TEXT_GRID_HEIGHT = screen_height / 19;
  smap_offset_x = screen_width - 219;
  smap_offset_y = screen_height - 239;
  NUM_MSG = (smap_offset_y - 110) / MESSAGE_HEIGHT;

  if (NUM_MSG > MAX_MSG) NUM_MSG = MAX_MSG;
  text_offset_x = (TEXT_GRID_WIDTH - 80) / 2;
  text_offset_y = (TEXT_GRID_HEIGHT - 25) / 2;
  trophy_count = 0;
  while (trophies[trophy_count].criteria_type < 999) trophy_count++;
  if (showStats) printf("Trophy Count: %d\n",trophy_count);
  i=0;
  while (objects[i].otype < 999) {
    if (objects[i].rarity > 2500) loot_commons++;
    if (objects[i].rarity > 500 && objects[i].rarity <= 2500) loot_uncommons++;
    if (objects[i].rarity > 100 && objects[i].rarity <= 500) loot_rares++;
    if (objects[i].rarity > 5 && objects[i].rarity <= 100) loot_very_rares++;
    if (objects[i].rarity > 0 && objects[i].rarity <= 5) loot_ultra_rares++;
    i++;
    }
  NO_OF_OBJECTS = i;
  TOTAL_OBJECTS = i;
  if (showStats) {
    printf("Object Count: %d\n",i);
    printf("  Common     : %d\n",loot_commons);
    printf("  Uncommon   : %d\n",loot_uncommons);
    printf("  Rare       : %d\n",loot_rares);
    printf("  Very Rare  : %d\n",loot_very_rares);
    printf("  Ultra Rare : %d\n",loot_ultra_rares);
    printf("  Non Drop   : %d\n",i - (loot_commons + loot_uncommons + loot_rares + loot_very_rares + loot_ultra_rares));
    }
  i=0;
  while (gen[i].region < 99) i++;
  NO_OF_GENS = i;
  if (showStats) printf("Message Count: %d\n",i);
  i=0;
  recipe_start = -1;
  while (lore[i].region < 99) {
    if (strncmp(lore[i].message,"Recipe:",7) == 0 && recipe_start < 0) recipe_start = i;
    i++;
    }
  NO_OF_LORE = i;
  if (showStats) printf("Lore Count: %d\n",i);
  i=0;
  while (recipes[i].object < 999) i++;
  recipe_count = i;
  if (showStats) printf("Recipe Count: %d\n",i);
  num_spells = 0;
  while (spells[num_spells].effect != 999) {
    if (spells[num_spells].number != num_spells) printf("Spell number mismatch: %d\n",num_spells);
    num_spells++;
    }
  if (showStats) printf("Spell Count: %d\n",num_spells);
  i = 0;
  while (monsters[i].speed < 900) i++;
  NO_OF_MONSTERS = i;
  if (showStats) printf("Monster Count: %d\n",NO_OF_MONSTERS);
  if (load_maps(maps,error,(Int32)sizeof(error)) == 0) fatal_error(error);
  if (showStats) printf("Map Count: %d\n",MAP_COUNT);
  if (showStats) {
    printf("Options:\r\n");
    printf("  Auto Pickup Gold: %c\r\n",auto_pickup_gold);
    printf("  Auto Pickup Torches: %c\r\n",auto_pickup_torch);
    printf("  Auto Open Doors: %c\r\n",auto_open_doors);
    printf("  Auto Search Walls: %c\r\n",auto_bump_walls);
    printf("  Dungeon Depth: %d\n",bottom);
    printf("  Dungeon Scaling: %c\n",dungeon_scaling);
    printf("  Monster Count: %d\n",monster_count);
    printf("  Party Combat Mode: %c\n",party_combat_mode);
    printf("  Delay (traps): %d\n",delay_trap);
    printf("  Delay (ranged): %d\n",delay_ranged);
    printf("  Delay (hit): %d\n",delay_hit);
    if (heroicMode == -1) printf("  Game Mode: Casual\n");
    if (heroicMode == 0) printf("  Game Mode: Normal\n");
    if (heroicMode == 2) printf("  Game Mode: Heroic\n");
    if (heroicMode == 4) printf("  Game Mode: Epic\n");
    if (fkey_mode == 'S') printf("  Fkeys: Spells\n");
    if (fkey_mode == 'W') printf("  Fkeys: Weapons\n");
    if (fkey_mode == 'G') printf("  Fkeys: Gear\n");
    printf("  Screen Size: %d x %d\n", screen_width, screen_height);
    printf("  Music: %c\n", music);
    printf("  Music Volume: %d\n", music_volume);
    printf("  Game Thread: %c\n", game_thread);
    printf("  Integer Scaling: %c\n", integer_scaling);
    printf("Macros:\n");
    for (i=0; i<12; i++)
      printf("  F%d = %s\n",i+1,macros[i]);
    }
  if (showStats != 0) return(0);
  setup->width = screen_width;
  setup->height = screen_height;
  setup->gameThread = (game_thread == 'Y') ? 1 : 0;
  setup->integerScaling = (integer_scaling == 'Y') ? 1 : 0;
  return 1;
  }

/* The game in its window: title screen, play, scores, until the player saves and leaves */
void run_game(GAME_HOST* host) {
  /* rand() keeps its seed per thread, and with GameThread the game does not run on configure_game()'s thread */
  u_randomize();
  graphics = ' ';
  read_sprites();
  init_graph(host);
  hall_position = -1;
  death_position = -1;
  num_hall_of_fame = 0;
  num_final_scores = 0;
  load_scores();
  set_music_volume();

  /* Death and giving up return to the title screen; saving ends the program. */
  do {
    play_music(MUSIC_INTRO);
    if (loadgame == 1) title();
    else {
      loadgame = title_menu();
      if (loadgame == 1) apply_character_config();
      }
    show_legends();
/*
    show_final_scores();
*/
    start_game(loadgame);
    loadgame = 0;
    play_game();
    end_game();
    } while (exit_flag != 'S');
  stop_music();
  }

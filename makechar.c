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

/* A world seed from 0 to 1,999,999,999 built from three rand() calls of 15 bits each */
static Int32 world_seed(void) {
  UInt32 s;

  s = ((UInt32)rand() << 30) ^ ((UInt32)rand() << 15) ^ (UInt32)rand();

  return (Int32)(s % 2000000000);
  }

void initial_inventory(Int32 slot,Int32 itemnum) {
  player.invent[slot] = *(find_linked_item(itemnum));
  player.invent[slot].flags |= OF_IDENT;
  if (player.invent[slot].otype == O_ARMR) player.armour += player.invent[slot].damage;
  if (player.invent[slot].otype == O_HELM) player.armour += player.invent[slot].damage;
  if (player.invent[slot].otype == O_BOOT) player.armour += player.invent[slot].damage;
  if (player.invent[slot].otype == O_ROBE) player.armour += player.invent[slot].damage;
  if (player.invent[slot].otype == O_EDGE) player.weapon += player.invent[slot].damage;
  if (player.invent[slot].otype == O_BLNT) player.weapon += player.invent[slot].damage;
  if (player.invent[slot].otype == O_POLE) player.weapon += player.invent[slot].damage;
  if (player.invent[slot].otype == O_DAGR) player.weapon += player.invent[slot].damage;
  if (player.invent[slot].otype == O_BOW) player.weapon += player.invent[slot].damage;
  }

void remove_from_inventory(Int32 link) {
  Int32 i;
  Int32 j;
  for (i=0; i<20; i++) if (player.invent[i].link == link) player.invent[i].otype = O_NONE;
  for (i=0; i<33; i++) if (player.pack[i].link == link) player.pack[i].otype = O_NONE;
  for (i=0; i<1000; i++) if (player.hole[i].link == link) player.hole[i].otype = O_NONE;
  for (i=0; i<5; i++)
    for (j=0; j<500; j++)
      if (player.houses[i].inventory[j].link == link) player.houses[i].inventory[j].otype = O_NONE;
  }

Int32 get_con_hp() {
  if (get_con() < 15) return 0;
  return ((get_con() - 10) / 3);
  }

#define CHOICE_LINE    15     /* the heading over each list of choices */
#define KEYS_LINE      25
#define CHOICE_KEYS    "(8/2) Select   (ENTER) Choose"

static char* sex_names[2] = { "Male", "Female" };

static void draw_sex(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,sex_names[row],"");
  }

static void draw_race(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,races[row].name,"");
  }

static void draw_start(MENU* menu,Int32 row,Int32 line) {
  draw_menu_text(menu,line,classes[((Int32*)menu->screen)[row] - 1].name,"");
  }

/* A list of choices under the heading, for the Improved Interface preference */
static void open_choices(MENU* menu,Int32 count,void (*draw_row)(MENU* menu,Int32 row,Int32 line),void* screen) {
  menu->rows = (SCROLL_LIST){ .count = count, .shown = count, .line = CHOICE_LINE };
  menu->row = 0;
  menu->column = 4;
  menu->value_column = 30;
  menu->color = 15;
  menu->keys = CHOICE_KEYS;
  menu->keys_line = KEYS_LINE;
  menu->draw_row = draw_row;
  menu->screen = screen;
  draw_menu(menu);
  }

/* The key that picks the row chosen in the numbered list it replaces; ESC does not back out */
static char choose(MENU* menu) {
  char key;

  key = 27;
  while (key == 27) key = menu_number(menu);

  return key;
  }

static void clear_choice_keys(void) {
  char text[81];

  sprintf(text,"%80s","");
  gotoxy(1,KEYS_LINE); txt_write(text);
  }

static char choose_sex(void) {
  MENU menu;
  char key;

  textcolor(14); gotoxy(10,3); txt_write("_");
  textcolor(9); gotoxy(5,CHOICE_LINE); txt_write("Character Sex:");
  open_choices(&menu,2,draw_sex,NULL);
  key = choose(&menu);

  gotoxy(5,CHOICE_LINE); txt_write("              ");
  gotoxy(8,CHOICE_LINE + 1); txt_write("      ");
  gotoxy(8,CHOICE_LINE + 2); txt_write("      ");

  return (key == '2') ? 'F' : 'M';
  }

void make_char(Int32 rebirth) {
  Int32 base_hp;
  Int32 con_hp;
  Int32 pool;
  Int32 i,j;
  Int32 x,y;
//  Int32 m_file;
  char buffer[80];
  Int32  starts[NUM_CLASSES];
  Int32  numStarts;
  char key;
  MENU menu;
  clrscr();
  char_sheet_labels('N');
  textcolor(14);
  if (rebirth == 0) {
    strcpy(player.name,"");
    while (strlen(player.name) < 2) {
      gotoxy(10,1); p_read(player.name,sizeof(player.name));
      }
    player.age = u_random(10)+18;
    } else {
    gotoxy(10,1); txt_write(player.name);
    }
  gotoxy(10,2);
  sprintf(buffer,"%d",player.age);
  txt_write(buffer);
  if (rebirth == 0 && improved_interface == 'Y') player.sex = choose_sex();
  else if (rebirth == 0) {
    textcolor(2); gotoxy(10,3); textcolor(14); txt_write("(m/f) _");
    key = inkey();
    if (key == 'f' || key == 'F') player.sex = 'F';
      else player.sex = 'M';
    }
  gotoxy(10,3); if (player.sex == 'F') txt_write("Female  ");
    else txt_write("Male    ");
  textcolor(9);
  player.health = 100;
  player.align = 100;
  player.armour = 0;
  player.weapon = 0;
  show_health();
  show_align();
  if (rebirth == 0) {
    gotoxy(5,15); txt_write("Character Race:");
    textcolor(15);

    if (improved_interface == 'Y') open_choices(&menu,NUM_RACES,draw_race,NULL);
    else {
      for (i=0; i<NUM_RACES; i++) {
        sprintf(buffer,"%d> %s",i+1,races[i].name);
        gotoxy(5,16+i); txt_write(buffer);
        }
      }
    textcolor(14);
    gotoxy(10,4); txt_write("_");
    key = '0';
    if (improved_interface == 'Y') key = choose(&menu);
    while (key<'1' || key>'0'+NUM_RACES) key = inkey();
    gotoxy(10,4);
    player.race = (key - '1' + 1);
    }
  textcolor(14);
  gotoxy(10,4);
  txt_write(races[player.race-1].name);
  gotoxy(5,15); txt_write("               ");
  gotoxy(5,16); txt_write("        ");
  gotoxy(5,17); txt_write("      ");
  gotoxy(5,18); txt_write("           ");
  gotoxy(5,19); txt_write("        ");
  gotoxy(5,20); txt_write("         ");
  player.level = 1; gotoxy(10,5);
  sprintf(buffer,"%d",player.level);
  txt_write(buffer);
  player.exp = 0; gotoxy(10,6);
  sprintf(buffer,"%9.0f",player.exp);
  txt_write(buffer);
  textcolor(9);
  gotoxy(5,15); txt_write("Character classes:");
  textcolor(15);
  numStarts = 0;
  for (i=0; i<NUM_CLASSES; i++)
    if (classes[i].flags & 2048) {
      if (improved_interface != 'Y') {
        gotoxy(5,16+numStarts);
        sprintf(buffer,"%d> %s",numStarts+1,classes[i].name);
        txt_write(buffer);
        }
      starts[numStarts++] = i+1;
    }
  if (improved_interface == 'Y') open_choices(&menu,numStarts,draw_start,starts);
  gotoxy(10,7);
  textcolor(14);
  txt_write("_");
  gotoxy(10,7);
  key = '0';
  if (improved_interface == 'Y') {
    key = choose(&menu);
    clear_choice_keys();
    textcolor(14);
    gotoxy(10,7);
    }
  while (key<'1' || key>'0'+numStarts) key = inkey();
  if (rebirth == 0) {
    for (i=1; i<=19; i++) player.invent[i].otype = O_NONE;
    initial_inventory(I_LSHL,40);            /* Iron Rations sword */
    }
  player.class = starts[key - '1'];
  set_title();
  player.exp_next = classes[player.class-1].first_exp;
  txt_write(classes[player.class-1].name);
  if (rebirth == 0) {
    for (i=0; i<4; i++)
      if (classes[player.class-1].initial_inv_slot[i] > 0)
        initial_inventory(classes[player.class-1].initial_inv_slot[i],
                          classes[player.class-1].initial_inv_item[i]);
    }

  gotoxy(5,15); txt_write("                  ");
  for (i=0; i<numStarts; i++) {
    gotoxy(5,16+i); txt_write("                        ");
    }
  gotoxy(10,8); txt_write(player.title);
  char_sheet_labels('Y');
  textcolor(14);

/* ***************************************************** */
/* **** Setup player record data before continuing ***** */
/* ***************************************************** */
  for (i=1; i<=6; i++) player.stat[i] = 10;
  if (player.sex == 'F') {
    player.stat[1] = player.stat[1]-1;
    player.stat[6] = player.stat[6]+1;
    player.stat[2] = player.stat[2]+1;
    }
  for (i=0; i<6; i++) player.stat[i+1] += races[player.race-1].stat_adjust[i];
  for (i=1; i<=6; i++) {
    gotoxy(40,i);
    sprintf(buffer,"%2d",player.stat[i]);
    txt_write(buffer);
    }
  for (i=0; i<=5; i++) player.party[i].hp = 0;
  for (i=1; i<=30; i++) player.skill[i] = 0;
  con_hp = get_con_hp();
  base_hp = classes[player.class-1].hitdie + u_random(classes[player.class-1].hitdie);
  player.mhp = base_hp+con_hp;
  player.chp = player.mhp;
  if (classes[player.class-1].becomes_caster == 1)
    player.mmana = classes[player.class-1].mana;
  if (classes[player.class-1].random_skill_range != 0) {
    i = u_random(3);
    if (classes[player.class-1].random_skill[i] > 0) {
      if (classes[player.class-1].random_skill_range > 0)
        player.skill[classes[player.class-1].random_skill[i]] =
          u_random(classes[player.class-1].random_skill_range) * 100000;
      else
        player.skill[classes[player.class-1].random_skill[i]] =
          -classes[player.class-1].random_skill_range * 100000;
      }
    }
  for (i=0; i<4; i++)
    if (classes[player.class-1].fixed_skill[i] > 0) {
      if (classes[player.class-1].fixed_skill_range[i] > 0)
        player.skill[classes[player.class-1].fixed_skill[i]] =
          u_random(classes[player.class-1].fixed_skill_range[i]) * 100000;
      else
        player.skill[classes[player.class-1].fixed_skill[i]] =
          -classes[player.class-1].fixed_skill_range[i] * 100000;
        
      }
  for (i=0; i<4; i++)
    if (races[player.race-1].fixed_skill[i] > 0) {
      if (races[player.race-1].fixed_skill_range[i] > 0)
        player.skill[races[player.race-1].fixed_skill[i]] +=
          u_random(races[player.race-1].fixed_skill_range[i]) * 100000;
      else
        player.skill[races[player.race-1].fixed_skill[i]] +=
          -races[player.race-1].fixed_skill_range[i] * 100000;
        
      }
  player.torch = 10;
  player.food = 5;
  player.time_played = 0;
  player.mount = 0;
  player.peg = 0;
  if (rebirth == 0) { for (i=0; i<255; i++) player.effect[i] = 0; }
  player.effect[PE_FIRE] = 0;
  player.effect[PE_ETHER] = 0;
  player.effect[PE_GIANT] = 0;
  player.effect[PE_HASTE] = 0;
  player.effect[PE_POISON] = 0;
  player.effect[PE_PROT] = 0;
  player.effect[PE_LIGHT] = 0;
  player.effect[PE_DEPTH] = bottom;
  player.effect[PE_NOARENA] = (no_arena == 'Y') ? 1 : 0;
  player.effect[PE_MCOUNT] = monster_count;
  player.effect[PE_HEROIC] = 0;

  /* Without the World Seed change u_random() scales a single rand() of 0-32767, so there are only 32,767 worlds */
  player.effect[PE_WORLDSEED] = (change_world_seed == 'Y') ? world_seed() : u_random(2000000000);
  if (heroicMode != 0) player.effect[PE_HEROIC] = heroicMode;
  player.in_city = 'N';
  player.hp_count = 15;
  player.mana_count = 20;
  player.effect[PE_READLANG] = 0;
  player.effect[PE_READLORE] = 0;
  player.phase = 0;
  player.effect[PE_SLEEP] = 0;
  player.effect[PE_BLIND] = 0;
  player.effect[PE_CONFUSED] = 0;
  player.effect[PE_FEAR] = 0;
  player.ship_x = 0;
  player.ship_y = 0;
  player.ship_r = 0;
  player.inship = ' ';
  player.gateway = ' ';
  if (rebirth == 0) player.gold_coins = 0;
  if (rebirth == 0) player.mithril_coins = 0;
  if (rebirth == 0) player.adamantite_coins = 0;
  if (rebirth == 0) player.bank = 0;
  player.interest = 0;
  for (i=0; i<255; i++) {
    stats.counts[i] = 0;
    if (rebirth == 0) {
      stats.trophies[i] = 0;
      stats.num_trophies = 0;
      }
    }
#ifdef MHR
#include "./Mhr/makechar.c"
#endif
  player.cmana = get_mmana();
  if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();


/* ************************************************ */
/* ***** Now finish getting input from player ***** */
/* ************************************************ */
  show_skills();
  char_sheet_data();
  textcolor(14);
  i = 1; pool = 18;
  gotoxy(46,i);
  sprintf(buffer,"<-%2d",pool);
  txt_write(buffer);
  key = 0;
  while (key != 13) {
    key = inkey();
    textcolor(14);
    switch (key) {
      case '8':if (i>1) {
                 gotoxy(46,i); txt_write("     ");
                 i = i-1;
                 gotoxy(46,i);
                 sprintf(buffer,"<-%2d ",pool);
                 txt_write(buffer);
                 }
               break;
      case '2':if (i<6) {
                 gotoxy(46,i); txt_write("     ");
                 i = i+1;
                 gotoxy(46,i);
                 sprintf(buffer,"<-%2d ",pool);
                 txt_write(buffer);
                 }
               break;
      case '4':if (pool>0) {
                 player.stat[i] = player.stat[i]+1;
                 pool = pool-1;
                 if (i == 3) {
                   con_hp = get_con_hp();
                   player.mhp = base_hp + con_hp;
                   player.chp = player.mhp;
                   }
/*
                 gotoxy(43,i);
                 sprintf(buffer,"%2d",player.stat[i]);
                 txt_write(buffer);
*/
                 if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();
                 adjust_abilities();
                 char_sheet_data();
                 textcolor(14);
                 gotoxy(46,i);
                 sprintf(buffer,"<-%2d ",pool);
                 txt_write(buffer);
                 }
               break;
      case '6':if (player.stat[i]>8) {
                 player.stat[i] = player.stat[i]-1;
                 pool = pool+1;
                 if (i == 3) {
                   con_hp = get_con_hp();
                   player.mhp = base_hp + con_hp;
                   player.chp = player.mhp;
                   }
/*
                 gotoxy(43,i);
                 sprintf(buffer,"%2d",player.stat[i]);
                 txt_write(buffer);
*/
                 if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();
                 adjust_abilities();
                 char_sheet_data();
                 textcolor(14);
                 gotoxy(46,i);
                 sprintf(buffer,"<-%2d ",pool);
                 txt_write(buffer);
                 }
        }
      if (key == 13 && pool != 0) key = 0;
    }
/*
  gotoxy(46,i); txt_write("     ");
  for (i=1; i<=6; i++) {
    gotoxy(40,i);
    sprintf(buffer,"%2d ",player.stat[i]);
    txt_write(buffer);
    }





  if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();
  gotoxy(40,7);
  sprintf(buffer,"%4d",player.chp);
  txt_write(buffer);
  gotoxy(40,8);
  sprintf(buffer,"%4d",player.mhp);
  txt_write(buffer);
  gotoxy(40,9);
  sprintf(buffer,"%4d",player.cmana);
  txt_write(buffer);
  gotoxy(40,10);
  sprintf(buffer,"%4d",player.mmana);
  txt_write(buffer);
  adjust_abilities();
  display_abilities();

  textcolor(2);
  gotoxy(1,12); txt_write("Torches:");
  gotoxy(1,13); txt_write("Gold   :");
  gotoxy(31,12); txt_write("Mount  :");
  gotoxy(31,13); txt_write("Winged :");
  gotoxy(55,12); txt_write("Armor  :");
  gotoxy(55,13); txt_write("Weapon :");
  textcolor(14);
  gotoxy(10,12); 
  sprintf(buffer,"%d",player.torch);
  txt_write(buffer);
  gotoxy(10,13);
  sprintf(buffer,"%d",get_gold());
  txt_write(buffer);
  gotoxy(40,12); if (player.mount == 0) txt_write("No"); else txt_write("Yes");
  gotoxy(40,13); if (player.peg == 0) txt_write("No"); else txt_write("Yes");
  gotoxy(64,12);
  sprintf(buffer,"%d",player.armour);
  txt_write(buffer);
  gotoxy(64,13);
  sprintf(buffer,"%d",player.weapon);
  txt_write(buffer);
  gotoxy(1,16); if (get_skill(1)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Arrow",get_skill(1)); txt_write(buffer);
  gotoxy(1,17); if (get_skill(2)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Edge Weapons",get_skill(2)); txt_write(buffer);
  gotoxy(1,18); if (get_skill(3)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Pole Weapons",get_skill(3)); txt_write(buffer);
  gotoxy(1,19); if (get_skill(4)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Hunting",get_skill(4)); txt_write(buffer);
  gotoxy(20,16); if (get_skill(5)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Scouting",get_skill(5)); txt_write(buffer);
  gotoxy(20,17); if (get_skill(6)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Language",get_skill(6)); txt_write(buffer);
  gotoxy(20,18); if (get_skill(7)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Lore",get_skill(7)); txt_write(buffer);
  gotoxy(20,19); if (get_skill(8)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Magic",get_skill(8)); txt_write(buffer);
  gotoxy(40,16); if (get_skill(9)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Healing",get_skill(9)); txt_write(buffer);
  gotoxy(40,17); if (get_skill(10)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Medicine",get_skill(10)); txt_write(buffer);
  gotoxy(40,18); if (get_skill(11)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Riding",get_skill(11)); txt_write(buffer);
  gotoxy(40,19); if (get_skill(12)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Boating",get_skill(12)); txt_write(buffer);
  gotoxy(60,16); if (get_skill(13)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Oracle",get_skill(13)); txt_write(buffer);
  gotoxy(60,17); if (get_skill(14)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Charm",get_skill(14)); txt_write(buffer);
  gotoxy(60,18); if (get_skill(15)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Survival",get_skill(15)); txt_write(buffer);
  gotoxy(60,19); if (get_skill(16)>4) textcolor(14); else textcolor(8);
  sprintf(buffer,"%2d Combat",get_skill(16)); txt_write(buffer);
*/
  textcolor(14);
  player.map_x = 67;player.map_y = 10;
  player.land_x = 73; player.land_y = 16;
  player.region = 9;
  for (x=1; x<=100; x++)
    for (y=1; y<=100; y++)
      player.known[x][y] = 0;
//#ifdef MINGW
//  m_file = open("temp01.$$$",O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,0660);
//#else
//  m_file = open("temp01.$$$",O_WRONLY | O_CREAT | O_TRUNC,0660);
//#endif
//  if (m_file < 0) {
//    printf("Fatal Error: Could not open temp01.$$$\n");
//    exit(1);
//    }
  for (i=1; i<=NO_OF_MAPS; i++) {
//    lseek(m_file,sizeof(KNOW_TYPE) * i,SEEK_SET);
//    write(m_file,&(player.known),sizeof(KNOW_TYPE));
    memcpy(&(known_maps[i]), &(player.known), sizeof(KNOW_TYPE));
    }
//  close(m_file);
  player.day = 1; player.month = 1; player.hour = 8; player.min = 1;
  for (i=0; i<1000; i++) spellbook[i] = 0;
  if (rebirth == 0) {
    for (i=1; i<=32; i++) player.pack[i].otype = O_NONE;
    for (i=0; i<1000; i++) player.hole[i].otype = O_NONE;
    for (i=0; i<5; i++) {
      player.houses[i].region = 0;
      for (j=0; j<500; j++) player.houses[i].inventory[j].otype = O_NONE;
      }
    }
  for (i=1; i<5000; i++) player.gen[i] = 0;
  for (i=1; i<5000; i++) player.lore[i] = 0;
  for (i=1; i<=12; i++) strcpy(player.fspell[i],"");
  for (i=0; i<1000; i++) player.quests[i] = 'N';
  for (i=1; i<=10; i++) {
    player.tardis_x[i] = 0;
    player.tardis_y[i] = 0;
    player.tardis_r[i] = 0;
    strcpy(player.tardis_d[i],"");
    }
  for (i=0; i<10; i++) {
    strcpy(gear_manager[i].main_hand,"");
    strcpy(gear_manager[i].off_hand,"");
    strcpy(gear_manager[i].neck_1,"");
    strcpy(gear_manager[i].neck_2,"");
    strcpy(gear_manager[i].shield,"");
    strcpy(gear_manager[i].armor,"");
    strcpy(gear_manager[i].ring_l1,"");
    strcpy(gear_manager[i].ring_l2,"");
    strcpy(gear_manager[i].ring_r1,"");
    strcpy(gear_manager[i].ring_r2,"");
    strcpy(gear_manager[i].cloak,"");
    strcpy(gear_manager[i].boots,"");
    strcpy(gear_manager[i].helm,"");
    }
  if (classes[player.class-1].flags & CF_CASTER) {
    j = 1;
    i = 0;
    while (spells[i].effect < 900) {
      if (spells[i].allign == player.class && spells[i].level == 1) spellbook[j++] = i;
      i++;
      }
/*    new_spell();   */
    }
  if (rebirth != 0) player.effect[PE_REBIRTH]++;
  start_time = (Int32)time(NULL);
  new_pawn();
  if (rebirth != 0) {
    i = 0;
    while (links[i].region != 99) {
      if (links[i].treasure > 0 && links[i].treasure < 1000) remove_from_inventory(links[i].treasure);
      i++;
      }
    while (is_weapon(&(player.invent[I_LHND]))) inventory(NULL);
    }
  disp_char();
  }

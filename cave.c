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

static void cave_take_time(void) {
  player.min++;
  if (player.min > 59) {
    player.min = 0;
    player.hour++;
    player.health--;
    show_side();
    if (player.hour == 24) {
      player.hour = 0;
      player.day++;
      if (player.day == 32) {
        new_month();
        }
      }
    }
  } /* void cave_take_time() */

static Int32 barriers(void) {
  Int32 i;
  i = 0;
  if (player.map[player.x][player.y-1] == dungeon_wall) i |= 1;
  if (player.map[player.x+1][player.y] == dungeon_wall) i |= 2;
  if (player.map[player.x][player.y+1] == dungeon_wall) i |= 4;
  if (player.map[player.x-1][player.y] == dungeon_wall) i |= 8;
  return i;
  }

static void hit_enemy(void) {
  NPC_REC* monster;
/*    activeDelay = 0; */
  monster = get_monster(player.x,player.y);
  if (monster == NULL) return;
  if (is_weapon(&(player.invent[I_RHND]))) meleehit(0,&(player.invent[I_RHND]),get_weapon(),monster,'R');
    else meleehit(0,NULL,get_weapon(),monster,'R');
  if (is_weapon(&(player.invent[I_LHND])) && monster->hp > 0) meleehit(0,&(player.invent[I_LHND]),offhand_weapon() / 2,monster,'L');
  } /* void hit_enemy() */

static void search_wall(Int32 px,Int32 py,Int32 wall) {
  Int32 x,y;
  for (x=px-1; x<= px+1; x++)
    for (y=py-1; y<= py+1; y++)
      if (x>1 && x<99 && y>1 && y<99)
        if (player.map[x][y] == wall) {
          if (player.map[x-1][y] != wall && player.map[x+1][y] != wall) {
            player.map[x][y] = 22;
            player.known[x][y] = 0;
            add_skill(SK_SCOUT,1);
            }
          if (player.map[x][y-1] != wall && player.map[x][y+1] != wall) {
            player.map[x][y] = 22;
            player.known[x][y] = 0;
            add_skill(SK_SCOUT,1);
            }
          }
   return;



    if (player.map[px][py-1] == wall && player.map[px][py-2] != wall &&
        player.map[px-1][py-1] == wall && player.map[px+1][py-1] == wall &&
        player.map[px][py] != wall) {
      player.map[px][py-1] = 22;
      player.known[px][py-1] = 0;
      add_skill(SK_SCOUT,1);
      }
    if (player.map[px][py+1] == wall && player.map[px][py+2] != wall &&
        player.map[px-1][py+1] == wall && player.map[px+1][py+1] == wall &&
        player.map[px][py] != wall) {
      player.map[px][py+1] = 22;
      player.known[px][py+1] = 0;
      add_skill(SK_SCOUT,1);
      }
    if (player.map[px-1][py] == wall && player.map[px-2][py] != wall &&
        player.map[px-1][py-1] == wall && player.map[px-1][py+1] == wall &&
        player.map[px][py] != wall) {
      player.map[px-1][py] = 22;
      player.known[px-1][py] = 0;
      add_skill(SK_SCOUT,1);
    }
    if (player.map[px+1][py] == wall && player.map[px+2][py] != wall &&
        player.map[px+1][py-1] == wall && player.map[px+1][py+1] == wall &&
        player.map[px][py] != wall) {
      player.map[px+1][py] = 22;
      player.known[px+1][py] = 0;
      add_skill(SK_SCOUT,1);
      }
  }

static void search(void) {
  Int32 x,y;
  Int32 px,py;
  if (player.in_city == 'Y') return;
  px = player.x;
  py = player.y;
  for (x=player.x-1; x<= player.x+1; x++)
    for (y=player.y-1; y<= player.y+1; y++)
      if (player.map[x][y]>=800 && player.map[x][y]<900) {
          if (u_random(100) < (10+get_skill(SK_SCOUT)) || has_intrinsic(A_SRCH)) {
          player.map[x][y] = 40;
          player.known[x][y] = 0;
          add_skill(SK_SCOUT,1);
          }
      }
  if (u_random(100) < (10 + get_skill(SK_SCOUT)) || has_intrinsic(A_SRCH)) {
    search_wall(px,py,21);
    search_wall(px,py,38);
    search_wall(px,py,39);
    search_wall(px,py,41);
    }
  discover();
  if (u_random(10) == 1) add_skill(SK_SCOUT,1);
  } /* void search() */

static void attack(void) {
  Int32 xdir,ydir;
  char key;
  clear_prompt();
  prompt("Attack who ?");
  key = inkey();
  xdir = 0;
  ydir = 0;
  switch (key) {
    case '8':ydir = -1; break;
    case '2':ydir = 1; break;
    case '4':xdir = -1; break;
    case '6':xdir = 1; break;
    case '1':ydir = 1; xdir = -1; break;
    case '3':ydir = 1; xdir = 1; break;
    case '7':ydir = -1; xdir = -1; break;
    case '9':ydir = -1; xdir = 1; break;
    }
  player.x += xdir;
  player.y += ydir;
  if (player.map[player.x][player.y] < 0) {
    if (combat_mode == 'F' || (player.monster[2][0-player.map[player.x][player.y]].hp != 0))
      arena(0-player.map[player.x][player.y],7,barriers(),player.x,player.y);
    else hit_enemy();
    }
  player.x -= xdir;
  player.y -= ydir;
  } /* void attack() */

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
      if (use_from_ground == 'Y') player.map[player.x][player.y] = currentFloor;
      }
    perform_effect(item);
    }
  show_spot(player.x,player.y);
  show_player();
  show_side();
  }
*/

/* valid_move() for the player on foot, while in a ship. A forbidding square is not walked: testing it would wear it. */
static Int32 walkable_on_foot(Int32 x,Int32 y) {
  char inship;
  char flag;

  if (player.map[x][y] >= 600 && player.map[x][y] < 700) return 0;

  inship = player.inship;
  player.inship = ' ';
  flag = valid_move(x,y);
  player.inship = inship;

  return (flag == '*');
  }

static void move(Int32 xdir,Int32 ydir) {
  Int32 x,y;
  Int32 previous;
  Int32 ex,ey;
  Int32 blocked;
  last_target_x = 0;
  last_target_y = 0;
  remove_player();
  x = player.x;
  y = player.y;
  if (player.map[player.x+xdir][player.y+ydir] == 22 &&
      auto_open_doors == 'Y') {
      player.map[player.x+xdir][player.y+ydir] = 23;
      show_spot(player.x+xdir,player.y+ydir);
      show_player();
      return;
    }
  if (player.map[player.x+xdir][player.y+ydir] == 21 &&
      auto_bump_walls == 'Y') {
      search();
      show_player();
      return;
    }
  player.x += xdir;
  player.y += ydir;

  /* With Ship Improvements a ship that runs into walkable shore twice puts the player ashore, the ship in its dock */
  blocked = (valid_move(player.x,player.y) == ' ');
  if (steps_ashore(x,y,xdir,ydir,blocked && player.inship != ' ' && walkable_on_foot(player.x,player.y))) {
    blocked = 0;
    show_spot(x,y);
    show_spot(player.ship_x,player.ship_y);
    }
  if (blocked) {
    player.x -= xdir;
    player.y -= ydir;
    }
  else walk_aboard();
  if (player.map[player.x][player.y] < 0 &&
      (player.monster[1][0-player.map[player.x][player.y]].special & MS_FRIENDLY) == 0) {
    if (combat_mode == 'F' || (player.monster[2][0-player.map[player.x][player.y]].hp != 0)) {
      ex = player.x;
      ey = player.y;
      previous = player.monster[1][0-player.map[ex][ey]].prev;
      arena(0-player.map[player.x][player.y],7,barriers(),player.x,player.y);
      if (player.map[ex][ey] == 7) player.map[ex][ey] = previous;
      show_spot(ex,ey);
      }
    else {
      player.x = x;
      player.y = y;
      show_player();
      player.x += xdir;
      player.y += ydir;
      hit_enemy();
      }
    player.x = x;
    player.y = y;
    }
  if (player.effect[PE_ROOT] > 0) { player.x = x; player.y = y; }
  if (get_weight() > get_max_weight()) { player.x = x; player.y = y; msg("Overburdoned",15); }
  if (adjust_map() == 'Y') show_map();
  show_player();
  stats.counts[STAT_STEPS_TAKEN]++;
  check_autos();
  }

static void remove_bones(void) {
  }

static Int32 cave_level_cleared(void) {
  Int32 x,y;
  if (player.in_city == 'Y' || player.d_level < 1 || levelCleared) return 0;
  y = 0;
  for (x=1; x<=60; x++)
    if (player.monster[1][x].hp > 0 && (player.monster[1][x].special & MS_FRIENDLY) == 0) y++;
  return (y > 0) ? 0 : -1;
  }

static void down_level(Int32 check_clear) {
  Int32 i,j;
  if (player.map[player.x][player.y] == 24 ||
      player.map[player.x][player.y] == 5) {

    /* Without the Sun Stone change a Sun Stone return saves the town over the level above */
    if (player.d_level > 0 && (check_clear || change_sun_stone != 'Y')) {
      save_level();
      if (current_dungeon.level[player.d_level-1].cleared == 0) {
        if (cave_level_cleared()) {
          current_dungeon.level[player.d_level-1].cleared = -1;
          stats.counts[STAT_LEVELS_CLEARED]++;
          j = -1;
          for (i=0; i<current_dungeon.num_levels; i++)
            if (current_dungeon.level[i].cleared == 0) j = 0;
          if (j) stats.counts[STAT_DUNGEON_CLEARED]++;
          }
        }
      }
    player.d_level++;
    make_map('D');
    levelCleared = cave_level_cleared();
    player.map_x = player.x - TILES_X/2;
    player.map_y = player.y - TILES_Y/2;
    if (player.map_x < 1) player.map_x = 1;
    if (player.map_y < 1) player.map_y = 1;
    if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
    if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
    show_screen();
/*      activeDelay = 2; */
    if (in_room(player.x,player.y) && can_see()) light_room();
    }
  } /* void down_level() */

static void up_level(void) {
  Int32 i,j;
  if (player.map[player.x][player.y] == 25) {
    save_level();
    if (current_dungeon.level[player.d_level-1].cleared == 0) {
      if (cave_level_cleared()) {
        current_dungeon.level[player.d_level-1].cleared = -1;
        stats.counts[STAT_LEVELS_CLEARED]++;
        j = -1;
        for (i=0; i<current_dungeon.num_levels; i++)
          if (current_dungeon.level[i].cleared == 0) j = 0;
        if (j) stats.counts[STAT_DUNGEON_CLEARED]++;
        }
      }
    player.d_level--;
    if (player.d_level != 0) {
      make_map('U');
      levelCleared = cave_level_cleared();
      }
    player.map_x = player.x - TILES_X/2;
    player.map_y = player.y - TILES_Y/2;
    if (player.map_x < 1) player.map_x = 1;
    if (player.map_y < 1) player.map_y = 1;
    if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
    if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
    if (player.d_level != 0) show_screen();
/*      activeDelay = 2; */
    if (in_room(player.x,player.y) && can_see()) light_room();
    }
  } /* void up_level() */

static void do_inventory(void) {
  saveScreen();
  inventory(NULL);
  restoreScreen();
  show_map();
  show_player();
  auto_detect();
  } /* void do_inventory() */

static void talk(void) {
  char key;
  char flag;
  Int32  i;
  NPC_REC* monster;
  Int32  xdir,ydir;
  key = item_dir(-1);
  if (key == ' ') {
    clear_prompt();
    prompt("Talk to who?");
    flag = ' ';
    while (flag == ' ') {
      key = inkey();
      if (key >= '1' && key < '5') flag = '*';
      if (key > '5' && key <= '9') flag = '*';
      }
    }
  xdir = 0;
  ydir = 0;
  switch (key) {
    case '2':ydir = 1; break;
    case '8':ydir = -1; break;
    case '4':xdir = -1; break;
    case '6':xdir = 1; break;
    case '1':xdir = -1; ydir = 1; break;
    case '3':xdir = 1; ydir = 1; break;
    case '7':xdir = -1; ydir = -1; break;
    case '9':xdir = 1; ydir = -1; break;
    }
  monster = get_monster(player.x + xdir,player.y + ydir);
  if (monster == NULL) {
    clear_prompt();
    msg("No one there ! ! !",14);
    }
  if (monster != NULL) {
    if ((monster->special & MS_FRIENDLY) == 0) {
      clear_prompt();
      prompt("Die Scum!!!");
      }
    else {
      if ((monster->special & MS_TALKED) == 0) {
        monster->special |= MS_TALKED;
        flag = ' ';
        i = 0;
        while (trophies[i].criteria_type < 999) {
          if (trophies[i].criteria_type == TC_TRICK) {
            if (player.invent[I_ARMR].link >= trophies[i].control[0] &&
                player.invent[I_ARMR].link <= trophies[i].control[1] &&
                (player.invent[I_RHND].link == trophies[i].control[2] ||
                 player.invent[I_LHND].link == trophies[i].control[2])
               ) {
              stats.counts[trophies[i].control[3]] |=
                   ( 1 << (player.invent[I_ARMR].link - trophies[i].control[0]));
              msg("Trick or Treat",15);
              special(84,NULL);
              flag = '*';
              }
            }
          i++;
          }
        if (flag == ' ' && player.in_city == 'Y' && player.city == 17) special(51,NULL);
          else special(20,NULL);
        stats.counts[STAT_CONVERSATIONS]++;
        }
      else {
        clear_prompt();
        msg("Hello!!",9);
        }
      }
    }
  } /* void talk() */

static void light_torch(void) {
  if (player.torch > 0) {
    player.torch--;
    player.effect[PE_LIGHT] = 2000;
    show_side();
    show_map();
    show_player();
    }
  } /* void light_torch() */

static void add_to_party(NPC_REC* monster,Int32 x,Int32 y,Int32 i) {
  Int32 pos;
  Int32 member;
  if (player.party[1].hp == 0) pos = 1;
    else if (player.party[2].hp == 0) pos = 2;
    else if (player.party[3].hp == 0) pos = 3;
    else if (player.party[4].hp == 0) pos = 4;
    else if (player.party[5].hp == 0) pos = 5;
    else pos = 0;
  if (pos>0) {
    clear_prompt();
    msg("I would be Honored!!!",15);
    monster->special |= MS_FRIENDLY;
    player.party[pos] = *monster;
    player.party[pos].exp = needed(player.party[pos].level);
    player.party[pos].time = -1;
    player.party[pos].hp = player.party[pos].level * 9;

    /* Without the Party Joining change these go to party[i], i being cave()'s loop variable */
    member = (change_party_joining == 'Y') ? pos : i;
    player.party[member].attack = 2 * player.party[member].level;
    player.party[member].defense = 2 * player.party[member].level;
    if (player.party[member].mana > 0)
      player.party[member].mana = 25 * player.party[member].level;
    player.map[x][y] = player.party[pos].prev;
    if (player.effect[PE_HEROIC] > 0) {
      player.party[pos].hp *= player.effect[PE_HEROIC];
      }
    show_spot(x,y);
    }
  else {
   clear_prompt();
   msg("Your party seems full as it is!!",14);
   }
  }

static void join(Int32 i) {
  char key;
  char flag;
  NPC_REC*  monster;
  Int32  xdir,ydir;
  key = item_dir(-1);
  if (key == ' ') {
    clear_prompt();
    prompt("Ask who to join?");
    flag = ' ';
    while (flag == ' ') {
      key = inkey();
      if (key >= '1' && key < '5') flag = '*';
      if (key > '5' && key <= '9') flag = '*';
      }
    }
  xdir = 0;
  ydir = 0;
  switch (key) {
    case '2':ydir = 1; break;
    case '8':ydir = -1; break;
    case '4':xdir = -1; break;
    case '6':xdir = 1; break;
    case '1':xdir = -1; ydir = 1; break;
    case '3':xdir = 1; ydir = 1; break;
    case '7':xdir = -1; ydir = -1; break;
    case '9':xdir = 1; ydir = -1; break;
    }
  monster = get_monster(player.x + xdir,player.y + ydir);
  if (monster == NULL) {
    clear_prompt();
    msg("No one there ! ! !",14);
    }
  if (monster != NULL) {
    if ((monster->special2 & M2_PACIFIED) != 0) {
      if (u_random(100)<(50+(player.level-monster->level)*10)) {
        add_to_party(monster,player.x+xdir,player.y+ydir,i);
        }
      else {
          clear_prompt();
          msg("No Thanks.",14);
        }
      }
    else if ((monster->special & MS_FRIENDLY) == 0) {
        clear_prompt();
        msg("No Way Scum!!!",14);
      }
    else
      if ((monster->special & MS_JOIN) == 0) {
        monster->special |= MS_JOIN;
          if (u_random(100)<(50+(player.level-monster->level)*10)) {
            add_to_party(monster,player.x+xdir,player.y+ydir,i);
            }
          else {
              clear_prompt();
              msg("No Thanks.",14);
            }
        }
      else {
          clear_prompt();
          msg("No Thanks.",14);
        }
  }
  check_mount();
  } /* void join() */

/* Whether the player stands beside shops[i]'s counter. Village shops are listed under region 0 at (0,0). */
static char beside_shop(Int32 i) {
  char here;

  if (change_village_shops == 'Y' && in_village() == 'Y') here = (shops[i].region == 0) ? 'Y' : 'N';
    else here = (player.region == shops[i].region &&
                 player.land_x == shops[i].rx &&
                 player.land_y == shops[i].ry) ? 'Y' : 'N';
  if (here == 'N') return 'N';

  return (player.x >= shops[i].cx-1 &&
          player.x <= shops[i].cx+1 &&
          player.y >= shops[i].cy-1 &&
          player.y <= shops[i].cy+1) ? 'Y' : 'N';
  }

/* The shop you stand beside, or -1 */
Int32 merchant_beside(void) {
  Int32 i;

  for (i=0; shops[i].shoptype != 999; i++)
    if (beside_shop(i) == 'Y') return i;

  return -1;
  }

static Int32 is_merchant(void) {
  return (merchant_beside() >= 0) ? -1 : 0;
  }

/* Whether a house for sale is the house owned in a house slot */
static Int32 same_house(FORSALE* sale,HOUSE* house) {
  return (sale->region == house->region &&
          sale->rx == house->rx &&
          sale->ry == house->ry &&
          sale->cx == house->cx &&
          sale->cy == house->cy);
  }

static void buy_item(Int32* i) {
  Int32 flag;
  Int32 x,y;
  Int32 price;
  OBJECT_REC item;
  OBJECT_REC* pitem;
  flag = 0;
  for (y=player.y-1; y<=player.y+1; y++)
    for (x=player.x-1; x<=player.x+1; x++)
      if (player.map[x][y] == 536) flag = -1;
  if (flag == -1) {
    pitem = NULL;
    if (u_random(1000) < 10) pitem = find_linked_item(532);
    else if (u_random(1000) < 100) pitem = find_linked_item(533);
    else if (u_random(1000) < 500) pitem = find_linked_item(535);
    else pitem = find_linked_item(534);
    price = 1000;
    if (player.in_city == 'N') {
      price = 300 - (player.d_level * 10);
      }
    if (price < 200) price = 200;
    if (get_gold() < price) {
      msg("Not enough gold",14);
      pitem = NULL;
      }
    if (pitem != NULL) {
      sub_gold(price);
      item = *pitem;
      item.flags |= OF_IDENT;
      inventory(&item);
      show_screen();
      }
    }
  for (*i=0; *i<5; (*i)++) {
    if (player.region == player.houses[*i].region &&
        player.land_x == player.houses[*i].rx &&
        player.land_y == player.houses[*i].ry &&
        player.x >= player.houses[*i].cx-1 &&
        player.x <= player.houses[*i].cx+1 &&
        player.y >= player.houses[*i].cy-1 &&
        player.y <= player.houses[*i].cy+1) {
      saveScreen();
      house_inventory(*i);
      restoreScreen();
      return;
      }
    }
  flag = -1;
  *i = 0;
  while (flag == -1) {
    if (player.region == forsale[*i].region &&
        player.land_x == forsale[*i].rx &&
        player.land_y == forsale[*i].ry &&
        player.x >= forsale[*i].cx-1 &&
        player.x <= forsale[*i].cx+1 &&
        player.y >= forsale[*i].cy-1 &&
        player.y <= forsale[*i].cy+1) flag = *i;
    if (forsale[*i].region == 999) flag = -2;
    (*i)++;
    }

  /* Without the House Buying change for-sale row n is checked against house slot n, wherever you stand */
  for (*i=0; *i<5; (*i)++) {
    if (change_house_buying == 'Y') {
      if (flag >= 0 && same_house(&forsale[flag],&player.houses[*i])) flag = -1;
      }
    else if (same_house(&forsale[*i],&player.houses[*i])) flag = -1;
    }
  if (flag >= 0) {
    saveScreen();
    buy_house(flag);
    restoreScreen();
    return;
    }

  flag = -1;
  *i = 0;
  while (flag == -1) {
    if (beside_shop(*i) == 'Y') flag = *i;
    if (shops[*i].shoptype == 999) flag = -2;
    (*i)++;
    }
  if (flag >= 0) {
    saveScreen();
    switch (shops[flag].shoptype) {
      case  0:shop(shops[flag].name,SHOP_WEAP); break;
      case  1:shop(shops[flag].name,SHOP_ARMR); break;
      case  2:shop(shops[flag].name,SHOP_ALCH); break;
      case  3:shop(shops[flag].name,SHOP_JEWL); break;
      case  4:shop(shops[flag].name,SHOP_BOOK); break;
      case  5:shop(shops[flag].name,SHOP_ROBE); break;
      case  6:shop(shops[flag].name,SHOP_MAGC); break;
      case  7:library(shops[flag].name); break;
      case  8:tavern(shops[flag].name); break;
      case  9:school(shops[flag].name); break;
      case 10:travel(shops[flag].name); break;
      case 11:shop(shops[flag].name,SHOP_FOOD); break;
      case 12:pawn(shops[flag].name,get_city()); break;
      case 13:temple(shops[flag].name); break;
      case 14:cavalier(shops[flag].name); break;
      case 15:mounts(shops[flag].name); break;
      case 16:bank(shops[flag].name); break;
      case 17:shop(shops[flag].name,SHOP_CRFT); break;
      case 18:shop(shops[flag].name,SHOP_SPEL); break;
      case 19:shop(shops[flag].name,SHOP_RECP); break;
      }
/*      show_screen(); */
    restoreScreen();
    show_player();
    }
  } /* void buy_item() */

static void find_closest_mob(Int32* x,Int32* y) {
  Int32 dist;
  Int32 closestX;
  Int32 closestY;
  dist = 50000;
  closestX = 0;
  closestY = 0;
  for (*x=1; *x<100; (*x)++)
    for (*y=1; *y<100; (*y)++) {
      if (player.map[*x][*y] < 0) {
        if (sqr(*x-player.x) + sqr(*y-player.y) < dist) {
          dist = sqr(*x-player.x) + sqr(*y-player.y);
          closestX = *x;
          closestY = *y;
          }
        }
      }
  target_x = closestX;
  target_y = closestY;
  }

static void gps(void) {
  char buffer[20];
  sprintf(buffer,"Region    : %d",player.region);
  msg(buffer,15);
  sprintf(buffer,"Region X,Y: %d,%d",player.land_x,player.land_y);
  msg(buffer,15);
  sprintf(buffer,"X,Y       : %d,%d",player.x,player.y);
  msg(buffer,15);
  }

static Int32 check_in_corridor(void) {
  return (in_room(player.x, player.y)) ? 0 : -1;
/*
  Int32 walls;
  walls = 0;
  if (player.map[player.x-1][player.y] == 21) walls++;
  if (player.map[player.x+1][player.y] == 21) walls++;
  if (player.map[player.x][player.y-1] == 21) walls++;
  if (player.map[player.x][player.y+1] == 21) walls++;
  return (walls >= 2) ? -1 : 0;
*/
  }

static void run(Int32* in_corridor) {
  msg("Run dir?",14);
  get_dir(&run_x,&run_y);
  *in_corridor = check_in_corridor();
  }

static void check_stop_running(Int32* in_corridor) {
  Int32 x;
  Int32 y;
  if (valid_move(player.x+run_x,player.y+run_y) == ' ') {
    run_x = 0;
    run_y = 0;
    return;
    }
  if (player.map[player.x+run_x][player.y+run_y] == 24 ||
      player.map[player.x+run_x][player.y+run_y] == 25) {
    run_x = 0;
    run_y = 0;
    return;
    }
  for (y=player.y-1; y<= player.y+1; y++)
    for (x=player.x-1; x<= player.x+1; x++) {
      if (player.map[x][y] < 0) {
        run_x = 0;
        run_y = 0;
        return;
        }
      }
  if (!*in_corridor && check_in_corridor()) *in_corridor = -1;
  if (*in_corridor && !check_in_corridor()) {
    run_x = 0;
    run_y = 0;
    }
  }

void cave() {
  char haste_flag;
  Int32  i;
//  Int32  m_file = 0;
  Int32  x,y;
  Int32  sx,sy;
  double d;
  byte key;
  Int32 dmg;
  char buffer[20];
  Int32  in_corridor = 0;
  Int32  command;

/*  Int32  activeDelay; */

  /* **************************************** */
  /* ***** Actual start of dungeon code ***** */
  /* **************************************** */
  play_music((player.in_city == 'Y') ? MUSIC_TOWN : MUSIC_DUNGEON);
  currentFloor = 7;
  dungeon_floor = currentFloor;
  dungeon_wall = 21;
  levelCleared = 0;
  dc = 1;
  if (player.in_city == 'Y') {
    check_noble();
    }
  check_mount();
  word_counter = 0;
  haste_flag = ' ';
  clair = ' ';
  napping = ' ';
  graphmode();
  if (used_sunstone == 'Y') {
    player.x = 20; player.y = 20;
    player.map[player.x][player.y] = 24;
    player.d_level--;
    down_level(0);
    used_sunstone = 'N';
    }
  else if (mod_exit == 2 && player.d_level == 0) {
      player.x = 20; player.y = 20;
    player.map[player.x][player.y] = 24;
      player.d_level = 0;
      down_level(0);
    }
  if (mod_exit == 5) {
      player.d_level = 1;
      copyMap(&player.map,player.city);
      place_town_ship();
//#ifdef MINGW
//      m_file = open("temp01.$$$",O_RDONLY | O_BINARY);
//#else
//      m_file = open("temp01.$$$",O_RDONLY);
//#endif
//      if (m_file < 0) {
//        printf("Fatal Error: Could not open temp01.$$$\n");
//        exit(1);
//        }
//      lseek(m_file,sizeof(KNOW_TYPE) * player.city,SEEK_SET);
//      read(m_file,&(player.known),sizeof(KNOW_TYPE));
//      close(m_file);
      memcpy(&(player.known), &(known_maps[player.city]), sizeof(KNOW_TYPE));
      i = 0;
      player.x = 50; player.y = 2;
      while (i >= 0) {
        if (links[i].region >= 99) i = -1;
        else if (links[i].region == player.region &&
            links[i].x      == player.land_x &&
            links[i].y      == player.land_y) {
          player.x = links[i].sx;
          player.y = links[i].sy;
          i = -1;
          } else i++;
        }
      ship_docks_in_town();
      stock_town(player.city);
      player.map_x = player.x-TILES_X/2;
      player.map_y = player.y-TILES_Y/2;
      if (player.map_x<1) player.map_x = 1;
      if (player.map_y<1) player.map_y = 1;
      if (player.map_x>100-TILES_X) player.map_x = 100-TILES_X;
      if (player.map_y>100-TILES_Y) player.map_y = 100-TILES_Y;

      i = 0;
      while (i >= 0) {
        if (shops[i].shoptype != 999) {
          if (shops[i].region == player.region &&
              shops[i].rx == player.land_x &&
              shops[i].ry == player.land_y)
            player.map[shops[i].cx][shops[i].cy] = 520;
          i++;
          } else i = -1;
        }
      i = 0;
      while (i >= 0) {
        if (forsale[i].region != 999) {
          if (forsale[i].region == player.region &&
              forsale[i].rx == player.land_x &&
              forsale[i].ry == player.land_y)
            player.map[forsale[i].cx][forsale[i].cy] = S_SALE;
          i++;
          } else i = -1;
        }
      for (i=0; i<5; i++)
        if (player.houses[i].region == player.region &&
            player.houses[i].rx == player.land_x &&
            player.houses[i].ry == player.land_y)
          player.map[player.houses[i].cx][player.houses[i].cy] = S_SOLD;

    }
  player.map_x = player.x - TILES_X/2;
  player.map_y = player.y - TILES_Y/2;
  if (player.map_x < 1) player.map_x = 1;
  if (player.map_y < 1) player.map_y = 1;
  if (player.map_x > 100-TILES_X) player.map_x = 100-TILES_X;
  if (player.map_y > 100-TILES_Y) player.map_y = 100-TILES_Y;
  if (player.in_city == 'Y') {
    for (x=1; x<=100; x++)
      for (y=1; y<=100; y++)
        if (player.known[x][y] == 0) player.known[x][y] = 1;
    }
  show_map();
  show_sm_map();
  discover();
  show_side();
  show_player();
  auto_detect();
  exitf = 0;
  run_x = 0;
  run_y = 0;
  while (exitf == 0) {
     show_time();
     show_player();
     takes_time = 'Y';
     if (run_x != 0 || run_y != 0) check_stop_running(&in_corridor);
      if (player.effect[PE_SLEEP] != 0) key = ' ';
        else {
          if (run_x == -1 && run_y == 0) key = '4';
          if (run_x == 1 && run_y == 0) key = '6';
          if (run_x == 0 && run_y == -1) key = '8';
          if (run_x == 0 && run_y == 1) key = '2';
          if (run_x == -1 && run_y == -1) key = '7';
          if (run_x == 1 && run_y == -1) key = '9';
          if (run_x == -1 && run_y == 1) key = '1';
          if (run_x == 1 && run_y == 1) key = '3';
          if (run_x == 0 && run_y == 0) key = inkey();
          }

      /* TAB opens the Tab Menu, which types the command chosen, or loads another game */
      if (key == 9 && improved_interface == 'Y') {
        command = open_tab_menu();
        if (command == TAB_LOAD_GAME) exitf = 11;
        if (command <= 0) continue;
        key = (byte)command;
        }

      if (player.effect[PE_FEAR] > 0 &&
          ((key >= '1' && key <= '9') || key == 'f')) {
        find_closest_mob(&x,&y);
        key = fear_dir(player.x,player.y,target_x,target_y);
        }
      if (player.effect[PE_CONFUSED] > 0) {
        if (key == '1' || key == '2' || key == '3' || key == '4' ||
            key == '6' || key == '7' || key == '8' || key == '9') {
          switch (u_random(8)) {
            case 0:key = '1'; break;
            case 1:key = '2'; break;
            case 2:key = '3'; break;
            case 3:key = '4'; break;
            case 4:key = '6'; break;
            case 5:key = '7'; break;
            case 6:key = '8'; break;
            case 7:key = '9'; break;
            }
          }
        }
      switch (key) {
        case 'a':attack(); break;
        case 'Q':if (check_quit()) exitf = 1; break;
        case 'X':exitf = 4; break;
        case '8':move(0,-1); break;
        case '2':move(0,1); break;
        case '4':move(-1,0); break;
        case '6':move(1,0); break;
        case '7':move(-1,-1); break;
        case '9':move(1,-1); break;
        case '1':move(-1,1); break;
        case '3':move(1,1); break;
        case '5':run(&in_corridor); break;
        case '>':down_level(-1); break;
        case '<':up_level(); break;
        case '}':save_gear('N',-1); break;
        case ']':load_gear('N',-1); break;
        case '{':save_gear('Y',-1); break;
        case '[':load_gear('Y',-1); break;
        case 'e':eat(); break;
        case 's':search(); break;
        case 'H':show_icons();
                 takes_time = 'N';
                 break;
        case 'o':open_door(); break;
        case 'c':close_door(); break;
        case 'C':saveScreen(); crafting(); restoreScreen(); break;
        case 'i':do_inventory(); break;
        case 'g':do_get(); break;
        case 'G':if (player.map[player.x][player.y]>999) pack_it(7);
                 break;
        case 'q':show_quest(); break;
        case 'u':use_item(); break;
        case 'E':go_ship(); break;
        case 'f':fire_weapon(); break;
        case 'm':cast_spell(0); break;
        case KY_F1:fkey_pressed(1); break;
        case KY_F2:fkey_pressed(2); break;
        case KY_F3:fkey_pressed(3); break;
        case KY_F4:fkey_pressed(4); break;
        case KY_F5:fkey_pressed(5); break;
        case KY_F6:fkey_pressed(6); break;
        case KY_F7:fkey_pressed(7); break;
        case KY_F8:fkey_pressed(8); break;
        case KY_F9:fkey_pressed(9); break;
        case KY_F10:fkey_pressed(10); break;
        case KY_F11:fkey_pressed(11); break;
        case KY_F12:fkey_pressed(12); break;
        case KY_S_F1:shift_fkey_pressed(1); break;
        case KY_S_F2:shift_fkey_pressed(2); break;
        case KY_S_F3:shift_fkey_pressed(3); break;
        case KY_S_F4:shift_fkey_pressed(4); break;
        case KY_S_F5:shift_fkey_pressed(5); break;
        case KY_S_F6:shift_fkey_pressed(6); break;
        case KY_S_F7:shift_fkey_pressed(7); break;
        case KY_S_F8:shift_fkey_pressed(8); break;
        case KY_S_F9:shift_fkey_pressed(9); break;
        case KY_S_F10:shift_fkey_pressed(10); break;
        case KY_S_F11:shift_fkey_pressed(11); break;
        case KY_S_F12:shift_fkey_pressed(12); break;
        case KY_A_1:alt_key_pressed(1); break;
        case KY_A_2:alt_key_pressed(2); break;
        case KY_A_3:alt_key_pressed(3); break;
        case KY_A_4:alt_key_pressed(4); break;
        case KY_A_5:alt_key_pressed(5); break;
        case KY_A_6:alt_key_pressed(6); break;
        case KY_A_7:alt_key_pressed(7); break;
        case KY_A_8:alt_key_pressed(8); break;
        case KY_A_9:alt_key_pressed(9); break;
        case KY_A_0:alt_key_pressed(10); break;
        case KY_A_MINUS:alt_key_pressed(11); break;
        case KY_A_EQUAL:alt_key_pressed(12); break;
        case KY_C_F1:ctrl_fkey_pressed(1); break;
        case KY_C_F2:ctrl_fkey_pressed(2); break;
        case KY_C_F3:ctrl_fkey_pressed(3); break;
        case KY_C_F4:ctrl_fkey_pressed(4); break;
        case KY_C_F5:ctrl_fkey_pressed(5); break;
        case KY_C_F6:ctrl_fkey_pressed(6); break;
        case KY_C_F7:ctrl_fkey_pressed(7); break;
        case KY_C_F8:ctrl_fkey_pressed(8); break;
        case KY_C_F9:ctrl_fkey_pressed(9); break;
        case KY_C_F10:ctrl_fkey_pressed(10); break;
        case KY_C_F11:ctrl_fkey_pressed(11); break;
        case KY_C_F12:ctrl_fkey_pressed(12); break;
        case 'x':transcribe(); break;
        case 'n':nap(); break;
        case 't': if (is_merchant()) buy_item(&i); else talk();
                   break;
        case 'T':throw(' '); break;
        case '-':throw('L'); break;
        case '=':throw('R'); break;
        case '+':throw('R'); break;
        case 'j':join(i); break;
        case 'J':lock_door(); break;
        case 'p':party();
                 takes_time = 'N';
                 break;
        case 'P':if (player.map[player.x][player.y]>999) pack_it(7);
                 break;
        case 'l':light_torch(); break;
        case 'L':show_log();
                 takes_time = 'N';
                 break;
        case 'w':if (player.gateway != '*') {
                  if (word_counter == 0) say_word();
                  }
                 else {
                 clear_prompt();
                 msg("Words do Not work Here!!",12);
                 }
              break;
        case 'b':buy_item(&i); break;
        case 'S':
              saveScreen();
              save_new();
              hold_save(loadname);
              restoreScreen();
              break;
        case 'v':view(7); break;
        case 'V':soundon = (soundon == 'Y') ? 'N' : 'Y';
              break;
        case 'R':
              saveScreen();
              disp_char();
              restoreScreen();
              takes_time = 'N';
              break;
        case 'O':
              saveScreen();
              game_options();
              restoreScreen();
              show_map();
              show_player();
              takes_time = 'N';
              break;
        case 'h':healing(); break;
        case 'A':activate(player.x,player.y); break;
        case '?':dungeon_help();
                 takes_time = 'N';
                 break;
        case '~':gps();
                 takes_time = 'N';
                 break;
        case 'Z':show_number(); break;
        case ':':wizard(); break;
/*        case '^':pit_trap(); break; */
        }
    key = ' ';
    if (takes_time == 'Y') {
      discover();
      if (player.in_city != 'Y')
        if (in_room(player.x,player.y) && can_see()) light_room();
      if (player.align <= 0 && !(classes[player.class-1].flags & CF_ROGUE)) make_assassin();
      while (player.exp >= player.exp_next) { advancement(); show_side(); }
      if (has_intrinsic(A_TRAP) || u_random(100) < (get_skill(SK_SCOUT) / 5)) {
        for (x=player.x-1; x<=player.x+1; x++)
          for (y=player.y-1; y<=player.y+1; y++)
            if (x >= 1 && x<= 99 && y >= 1 && y <= 99)
              if (player.map[x][y] >= 800 && player.map[x][y] < 900) {
                player.known[x][y] = 1;
                player.map[x][y] = 40;
                show_spot(x,y);
                }
        }
      if (has_intrinsic(A_IDNT)) {
        if (player.map[player.x][player.y] >= 1000 && player.map[player.x][player.y] < 2000) {
          if ((player.map_objects[player.map[player.x][player.y]-1000].flags & OF_IDENT) == 0) {
            player.map_objects[player.map[player.x][player.y]-1000].flags |= OF_IDENT;
            msg(get_name(&(player.map_objects[player.map[player.x][player.y]-1000])),14);
            stats.counts[STAT_ITEMS_IDENTED]++;
            }
          }
        }
      /* Without the Sleep Traps change a sleep trap springs only under a sleeping player, who cannot step on it */
      if (player.map[player.x][player.y] == 801 && (change_sleep_traps == 'Y' || player.effect[PE_SLEEP] != 0)) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) sleep_trap();
        }
      if (player.map[player.x][player.y] == 802) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) poison_trap();
        }
      if (player.map[player.x][player.y] == 803) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) dart_trap();
        }
      if (player.map[player.x][player.y] == 804) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) teleport_trap();
        }
      if (player.map[player.x][player.y] == 805) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) fire_trap();
        }
      if (player.map[player.x][player.y] == 806) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) blind_trap();
        }
      if (player.map[player.x][player.y] == 807) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) root_trap();
        }
      if (player.map[player.x][player.y] == 808) {
        player.map[player.x][player.y] = 40;
        if (has_intrinsic(A_LEV) == 0) pit_trap();
        }
      if (player.map[player.x][player.y] == 41 ||
          player.map[player.x][player.y] == 145) {
        if (player.gateway == '*' || has_intrinsic(A_FCNG) == 0) {
          clear_prompt();
          msg("ZZZZAAAAPPPP!!!!!",12);
          stats_take_damage(player.chp / 2);
          player.chp = player.chp / 2;
          show_side();
          }
        }
      if (player.map[player.x][player.y] == 38) {
          clear_prompt();
          msg("SIZZLE!!!!!",12);
          stats_take_damage(player.chp / 2);
          player.chp = player.chp / 2;
          show_side();
        }
      if (player.map[player.x][player.y] == 39) {
          clear_prompt();
          msg("GLUB GLUB!!!",12);
          stats_take_damage(player.chp / 2);
          player.chp = player.chp / 2;
          show_side();
        }
      if (has_intrinsic(A_SRCH)) search();
      if (haste_flag == ' ') {
        if (player.party[0].hp > 0) move_summoned();
  /*      if (activeDelay > 0) activeDelay--; */
  /*      else { */
  
//            for (x=player.x-1; x<=player.x+1; x++)
//              for (y=player.y-1; y<=player.y+1; y++)
//                if (x>1 && x<100 && y>1 && y<100)
//                  if (player.map[x][y]<0)
//                    if ((player.monster[1][0-player.map[x][y]].special & MS_FRIENDLY) == 0 &&
//                         player.monster[1][0-player.map[x][y]].sleep <= 0) {
//  
//                      if (player.monster[2][0-player.map[x][y]].hp != 0) {
//                        previous = player.monster[1][0-player.map[x][y]].prev;
//                        arena(0-player.map[x][y],7,barriers(),x,y);
//                        if (player.map[x][y] == 7) player.map[x][y] = previous;
//                        }
//                        else enemy_hit(x,y);
//                      }
  
            for (x=1; x<100; x++)
              for (y=1; y<100; y++)
                if (x>0 && x<100 && y>0 && y<100) {
                  if (player.map[x][y]<0) {
                    if ((player.monster[1][0-player.map[x][y]].special & MS_MOVED) == 0)
                    if (player.monster[1][0-player.map[x][y]].sleep <= 0) {
                      if (player.monster[1][0-player.map[x][y]].speed < 0) {
                        if (can_be_seen() && los(player.x,player.y,x,y,'Y') != 0 &&
                            abs(player.x-x) < 20 && abs(player.y-y) < 20)
                          player.monster[1][0-player.map[x][y]].speed = 0-player.monster[1][0-player.map[x][y]].speed;
                        }
                      if ((player.monster[1][0-player.map[x][y]].mana != 0) &&
                          ((player.monster[1][0-player.map[x][y]].special & MS_FRIENDLY) == 0) &&
                          (player.monster[1][0-player.map[x][y]].speed >= 0))
                        if (u_random(100)<50) enemy_missile(x,y);
                      if (player.monster[1][0-player.map[x][y]].speed >= 0 &&
                          player.monster[1][0-player.map[x][y]].sleep == 0 &&
                          (player.monster[1][0-player.map[x][y]].special2 & M2_PACIFIED) == 0)
                        move_enemy(x,y);
                      }
                    }
                  else if (player.map[x][y] == 143) move_snake(x,y,dungeon_floor);
                  }
            for (x=1; x<=60; x++)
              if ((player.monster[1][x].special & MS_MOVED) == MS_MOVED)
                player.monster[1][x].special -= MS_MOVED;
  
  /*          }   */
  
          for (x=1; x<=5; x++) {
              if (player.party[x].sleep>0) player.party[x].sleep--;
              if (player.party[x].fear>0) player.party[x].fear--;
            }
          for (x=1; x<=60; x++) {
              if (player.monster[1][x].sleep>0) player.monster[1][x].sleep--;
              if (player.monster[1][x].sleep<0) player.monster[1][x].sleep++;
              if (player.monster[1][x].fear>0) {
                player.monster[1][x].fear--;
                if (player.monster[1][x].fear == 0) msg("Mob no longer scared",7);
                }
              if (player.monster[1][x].blind>0) player.monster[1][x].blind--;
              if (player.monster[1][x].confused>0) player.monster[1][x].confused--;
              if (player.monster[1][x].poison>0) {
  msg("here",15);
  sprintf(buffer,"poison=%d",player.monster[1][x].poison);
  msg(buffer,15);
                player.monster[1][x].poison--;
                player.monster[1][x].hp -= 2;
                poison_kill(&(player.monster[1][x]));
  msg("done",15);
                }
            }
        }
      if (haste_flag == '*')  haste_flag = ' ';
        else {
            if (has_intrinsic(A_SPD)) haste_flag = '*';
          }
      if (word_counter>0)  word_counter--;
      dc--;
      if (dc == 0) {
          if (player.effect[PE_PROT]>0) {
              player.effect[PE_PROT]--;
              if (player.effect[PE_PROT] == 0) {
                show_side();
                msg("Protection fades",7);
                }
            }
          if (player.effect[PE_FIRE]>0) {
              player.effect[PE_FIRE]--;
              if (player.effect[PE_FIRE] == 0)  show_side();
            }
          if (player.effect[PE_COLD]>0) {
              player.effect[PE_COLD]--;
              if (player.effect[PE_COLD] == 0)  show_side();
            }
          if (player.effect[PE_ELEC]>0) {
              player.effect[PE_ELEC]--;
              if (player.effect[PE_ELEC] == 0)  show_side();
            }
          if (player.effect[PE_ARCN]>0) {
              player.effect[PE_ARCN]--;
              if (player.effect[PE_ARCN] == 0)  show_side();
            }

          /* Without the Mind Shield change nothing counts PE_MIND down, so a Mind Shield never wears off */
          if (change_mind_shield == 'Y' && player.effect[PE_MIND]>0) {
              player.effect[PE_MIND]--;
              if (player.effect[PE_MIND] == 0)  show_side();
            }
          if (player.effect[PE_HASTE]>0) {
              player.effect[PE_HASTE]--;
              if (player.effect[PE_HASTE] == 0)  show_side();
            }
          if (player.effect[PE_GIANT]>0) {
              player.effect[PE_GIANT]--;
              if (player.effect[PE_GIANT] == 0)  show_side();
            }
          dc = 1+rcs_trunc(player.level/6.0);
        }
  
      if (classes[player.class-1].flags & CF_MONK)
        {
          if (player.level>=23)  player.effect[PE_HASTE] = 10;
          if (player.level>=17 && napping != '*')  player.effect[PE_SLEEP] = 0;
          if (player.level>=29)  player.effect[PE_POISON] = 0;
          if (player.level>=11)  player.effect[PE_FEAR] = 0;
          check_monk_weapon();
        }
        if (player.effect[PE_ETHER]>0) {
            player.effect[PE_ETHER]--;
            if (player.effect[PE_ETHER] == 0) show_side();
            if (player.effect[PE_ETHER] == 10) show_side();
          }
        if (player.effect[PE_POISON]>0) {
            dmg = player.effect[PE_POISON] / 10;
            if (dmg < 1) dmg = 1;
            player.effect[PE_POISON] -= dmg;
            sprintf(buffer,"%d poison damage",dmg);
            msg(buffer,13);
            stats_take_damage(dmg);
            player.chp -= dmg;
            show_side();
          }
        if (player.effect[PE_DOT]>0) {
            dmg = player.effect[PE_DOT] / 10;
            if (dmg < 1) dmg = 1;
            player.effect[PE_DOT] -= dmg;
            sprintf(buffer,"%d DOT damage",dmg);
            msg(buffer,12);
            stats_take_damage(dmg);
            player.chp -= dmg; show_side();
          }
        if (player.effect[PE_HCOUNT] > HEAL_TIME) player.effect[PE_HCOUNT] = HEAL_TIME;
        if (player.effect[PE_HCOUNT] > 0) player.effect[PE_HCOUNT]--;
        if (player.effect[PE_READLORE]>0) player.effect[PE_READLORE]--;
        if (player.effect[PE_READLANG]>0) player.effect[PE_READLANG]--;
        if (player.effect[PE_FEAR]>0) {
          player.effect[PE_FEAR]--;
          if (player.effect[PE_FEAR] == 0) msg("No longer afraid",14);
          }
        if (player.effect[PE_CONFUSED]>0) {
          player.effect[PE_CONFUSED]--;
          if (player.effect[PE_CONFUSED] == 0) msg("Your head clears",14);
          }
        if (player.effect[PE_LEV]>0) {
          player.effect[PE_LEV]--;
          if (player.effect[PE_LEV] == 0) show_side();
          }
        if (player.effect[PE_BLIND]>0) {
          player.effect[PE_BLIND]--;
          if (player.effect[PE_BLIND] == 0) show_map();
          }
        if (player.effect[PE_SLEEP]>0) {
            player.effect[PE_SLEEP]--;
            if (player.effect[PE_SLEEP] == 0) {
                if (napping == '*') napping = ' ';
                show_spot(player.x,player.y);
                show_player();
              }
          }
        if (player.effect[PE_ROOT]>0) {
            player.effect[PE_ROOT]--;
            if (player.effect[PE_ROOT] == 0) {
                show_spot(player.x,player.y);
                show_player();
              }
          }
        if (player.effect[PE_LIGHT]>0) {
            player.effect[PE_LIGHT]--;
            if (player.effect[PE_LIGHT] == 0) {
              run_x = 0;
              run_y = 0;
              show_side();
              show_map();
              }
          }
        tick_polymorph();
        if (player.phase>0) {
            player.phase--;
            if (player.phase == 0) {
                for (x=1; x<=100; x++)
                  for (y=1; y<=100; y++)
                    if (player.map[x][y] == 42) player.map[x][y] = 21;
                show_map();
              }
          }
        for (i=0; i<=5; i++) if (player.party[i].hp != 0)
          if (player.party[i].time>0) {
              player.party[i].time--;
              if (player.party[i].time == 0) {
                player.party[i].hp = 0;
                if (i == 0) {
                  x = player.party[0].treasure & 0xff;
                  y = (player.party[0].treasure >> 8) & 0xff;
                  player.map[x][y] = player.party[i].prev;
                  show_spot(x,y);
                  }
                }
            }
        player.hp_count -= get_hpr();
        if (player.hp_count<1) {
            player.hp_count = 50;
            if (player.chp<get_mhp()) player.chp = player.chp+1;
            show_side();
          }
        player.mana_count -= get_manar();
        if (player.mana_count<1) {
            player.mana_count = 100;
            if (player.cmana<get_mmana()) player.cmana = player.cmana+1;
            show_side();
          }
        cave_take_time();
        if (player.in_city == 'Y')
          if (player.x == 1 || player.x == 100 ||
              player.y == 1 || player.y == 100) {
//#ifdef MINGW
//              m_file = open("temp01.$$$",O_RDWR | O_BINARY);
//#else
//              m_file = open("temp01.$$$",O_RDWR);
//#endif
//              if (m_file < 0) {
//                printf("Fatal Error: Could not open temp01.$$$\n");
//                exit(1);
//                }
//              lseek(m_file,sizeof(KNOW_TYPE)*player.city,SEEK_SET);
//              write(m_file,&(player.known),sizeof(KNOW_TYPE));
//              close(m_file);
              memcpy(&(known_maps[player.city]), &(player.known), sizeof(KNOW_TYPE));
              exitf = 2;
            }
        if (has_intrinsic(A_DTUP) || has_intrinsic(A_DTDN)) {
          sx = -1; sy = -1;
          d = 99999.0;
          for (x=1; x<=100; x++)
            for (y=1; y<=100; y++)
              if ((player.map[x][y] == S_UPSTAIR && has_intrinsic(A_DTUP)) ||
                  (player.map[x][y] == S_DNSTAIR && has_intrinsic(A_DTDN)) ) {
                if ((x-player.x)*(x-player.x) + (y-player.y)*(y-player.y) < d) {
                  sx = x;
                  sy = y;
                  d = (x-player.x)*(x-player.x) + (y-player.y)*(y-player.y);
                  }
                }
          if (sx >= 0) {
            if (abs(player.x-sx)<4 && player.y<sy) x = 92;
            else if (abs(player.x-sx)<4 && player.y>sy) x = 91;
            else if (player.x<sx && abs(player.y-sy)<4) x = 93;
            else if (player.x>sx && abs(player.y-sy)<4) x = 94;
            else if (player.x<sx && player.y<sy) x = 97;
            else if (player.x>sx && player.y<sy) x = 98;
            else if (player.x<sx && player.y>sy) x = 95;
            else if (player.x>sx && player.y>sy) x = 96;
            else x = 10;
            putimage(300,screen_height-27,select_sprite(10),0);
            putimage(300,screen_height-27,select_sprite(x),0);
            } else
  		  putimage(300,screen_height-27,select_sprite(10),0);
          } else
  	       	putimage(300,screen_height-27,select_sprite(10),0);
        check_trophies();
        if (player.d_level == 0) exitf = 2;
        if (player.chp<1) exitf = 6;
        if (player.health<=0) exitf = 6;
      if (has_intrinsic(A_FOOD) && player.health < 100) player.health++;
      if (get_str() <= 0 || get_dex() <= 0 || get_int() <= 0 ||
          get_con() <= 0 || get_wis() <= 0 || get_chr() <= 0) exitf = 9;
      }
    show_floor_item();
    track_ship();
    }
  hide_bottom_name();
  /* ************************************ */
  /* ***** End of main dungeon loop ***** */
  /* ************************************ */
  if (exitf == 1) mod_exit = 3;
  if (exitf == 2) mod_exit = 1;
  if (exitf == 4) mod_exit = 4;
  if (exitf == 6) mod_exit = 6;
  if (exitf == 7) mod_exit = 7;
  if (exitf == 9) mod_exit = 9;
  if (exitf == 10) mod_exit = 2;
  if (exitf == 11) mod_exit = 8;
  remove_bones();
  }


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

void check_level_legacy() {
  if (player.invent[I_SHLD].otype == O_SHLD && player.invent[I_SHLD].effect == E_LGCY) level_legacy(&(player.invent[I_SHLD]));
  if (player.invent[I_ARMR].otype == O_ARMR && player.invent[I_ARMR].effect == E_LGCY) level_legacy(&(player.invent[I_ARMR]));
  if (player.invent[I_BOOT].otype == O_BOOT && player.invent[I_BOOT].effect == E_LGCY) level_legacy(&(player.invent[I_BOOT]));
  if (player.invent[I_HELM].otype == O_HELM && player.invent[I_HELM].effect == E_LGCY) level_legacy(&(player.invent[I_HELM]));
  if (player.invent[I_ROBE].otype == O_ROBE && player.invent[I_ROBE].effect == E_LGCY) level_legacy(&(player.invent[I_ROBE]));
  if (player.invent[I_RHND].otype == O_EDGE ||
      player.invent[I_RHND].otype == O_POLE ||
      player.invent[I_RHND].otype == O_DAGR ||
      player.invent[I_RHND].otype == O_BLNT ||
      player.invent[I_RHND].otype == O_BOW) {
    if (player.invent[I_RHND].effect == E_LGCY) level_legacy(&(player.invent[I_RHND]));
    }
  if (player.invent[I_LHND].otype == O_EDGE ||
      player.invent[I_LHND].otype == O_POLE ||
      player.invent[I_LHND].otype == O_DAGR ||
      player.invent[I_LHND].otype == O_BLNT ||
      player.invent[I_LHND].otype == O_BOW) {
    if (player.invent[I_LHND].effect == E_LGCY) level_legacy(&(player.invent[I_LHND]));
    }
  }

void player_attack_defense() {
  Int32 i;
  player.armour = 0;
  if (player.invent[I_SHLD].otype == O_SHLD && player.invent[I_SHLD].effect != E_PROT) player.armour = player.invent[I_SHLD].damage + player.invent[I_SHLD].plus * 3;
  if (is_weapon(&(player.invent[I_LHND]))) player.armour = 0;
  if (player.invent[I_RHND].otype == O_BOW || player.invent[I_RHND].otype == O_POLE) player.armour = 0;
  if (player.invent[I_RHND].otype == O_EDGE && player.invent[I_RHND].skill == SK_2HSWORD) player.armour = 0;
  if (player.invent[I_ARMR].otype == O_ARMR && player.invent[I_ARMR].effect != E_PROT)
     player.armour += player.invent[I_ARMR].damage +
                      player.invent[I_ARMR].plus * 5;
  if (player.invent[I_BOOT].otype == O_BOOT && player.invent[I_BOOT].effect != E_PROT)
     player.armour += player.invent[I_BOOT].damage +
                      player.invent[I_BOOT].plus * 2;
  if (player.invent[I_HELM].otype == O_HELM && player.invent[I_HELM].effect != E_PROT) 
      player.armour += player.invent[I_HELM].damage +
                       player.invent[I_HELM].plus * 2;
  if (player.invent[I_ROBE].otype == O_ROBE && player.invent[I_ROBE].effect != E_PROT) 
      player.armour += player.invent[I_ROBE].damage +
                       player.invent[I_ROBE].plus * 2;
  if (is_weapon(&(player.invent[I_RHND])))
    player.weapon = player.invent[I_RHND].damage;
    else player.weapon = 0;
  for (i=I_RNG1; i<=I_RNG4; i++)
    if (player.invent[i].otype != O_NONE)
      player.weapon += player.invent[i].damage;
  if (player.weapon == 0) player.weapon = (get_str() / 5) + 1;
  }

Int32 item_equipped(Int32 item) {
  if (player.invent[I_ARMR].link == item) return -1;
  if (player.invent[I_SHLD].link == item) return -1;
  if (player.invent[I_HELM].link == item) return -1;
  if (player.invent[I_BOOT].link == item) return -1;
  if (player.invent[I_ROBE].link == item) return -1;
  if (player.invent[I_RNG1].link == item) return -1;
  if (player.invent[I_RNG2].link == item) return -1;
  if (player.invent[I_NCK1].link == item) return -1;
  if (player.invent[I_NCK2].link == item) return -1;
  if (player.invent[I_BLTF].link == item) return -1;
  if (player.invent[I_BLTB].link == item) return -1;
  if (player.invent[I_BLTL].link == item) return -1;
  if (player.invent[I_BLTR].link == item) return -1;
  if (player.invent[I_RHND].link == item && is_weapon(&(player.invent[I_RHND]))) return -1;
  if (player.invent[I_LHND].link == item && is_weapon(&(player.invent[I_LHND]))) return -1;
  return 0;
  }

Int32 can_drop(Int32 destx,Int32 desty,char check_inair) {
  Int32 i;
  Int32 x,y;
  drop_x = -1;
  drop_y = -1;
  if (check_inair == 'Y' && in_air.otype == O_NONE) {
    drop_x = 0;
    drop_y = 0;
    return -1;
    }
  if (player.map[destx][desty] == currentFloor) {
    drop_x = destx;
    drop_y = desty;
    return -1;
    }
  for (i=1; i<=5; i++) {
    for (y=desty-i; y<=desty+i; y++)
      for (x=destx-i; x<=destx+i; x++)
        if (x >= 1 && x < 100 && y >= 1 && y < 100)
          if (player.map[x][y] == currentFloor) {
            drop_x = x;
            drop_y = y;
            return -1;
            }
    }
  return 0;
  }

#define NUM_SLOTS       20
#define NAME_WIDTH      24      /* a name in the list, which ends before the card */
#define ACTION_COLUMN   48      /* the actions for a row show under its card */
#define ACTION_LINE     19
#define INVENTORY_KEYS  "(8/2) Select   (ENTER) Actions   (G) Gear Sets   (ESC) Leave"
#define GEAR_KEYS       "(8/2) Select   (ENTER) Actions   (G/ESC) Back"
#define ACTION_KEYS     "QPTVvXxI"
#define NUM_GEAR_SETS   10
#define GEAR_LINE        3      /* the gear sets' heading, where the card shows; the sets follow */
#define GEAR_ACTION_LINE 15
#define HEADING_COLOR   11

/* Row 0 is the item in the air, shown as on the ground; rows 1 to 19 are the inventory's slots */
static char* slot_names[NUM_SLOTS] = {
  "On Ground", "Main Hand", "Off Hand", "Left Shoulder", "Right Shoulder", "Neck 1", "Neck 2", "Belt Left",
  "Belt Right", "Belt Front", "Belt Back", "Shield", "Armor", "Ring Left 1", "Ring Left 2", "Ring Right 1",
  "Ring Right 2", "Cloak", "Boots", "Helm"
  };

static OBJECT_REC* slot_item(Int32 row) {
  return (row > 0) ? &(player.invent[row]) : &in_air;
  }

static void slot_label(Int32 row,char* label) {
  sprintf(label,"%-15s:",slot_names[row]);
  }

Int32 pack_count(void) {
  Int32 i;
  Int32 count;

  count = 0;
  for (i=1; i<=PACK_SLOTS; i++)
    if (player.pack[i].otype != O_NONE) count++;

  return count;
  }

/* The treasure of the dungeon you are in, on its bottom level, which completes the dungeon's quest once picked up */
Int32 is_quest_treasure(OBJECT_REC* item) {
  OBJECT_REC* treasure;

  if (player.d_level != bottom) return 0;

  treasure = find_linked_item(links[player.dungeon_number].treasure);

  return (treasure != NULL && strcmp(item->name,treasure->name) == 0);
  }

/* A row's slot, then name in color */
static void draw_slot_name(MENU* menu,Int32 row,Int32 line,char* name,Int32 color) {
  char label[20];
  char text[NAME_WIDTH + 1];

  slot_label(row,label);
  sprintf(text,"%-*.*s",NAME_WIDTH,NAME_WIDTH,name);

  draw_menu_text(menu,line,label,"");
  list_text(&menu->rows,menu->value_column,line,color,text);
  }

static void draw_slot(MENU* menu,Int32 row,Int32 line) {
  OBJECT_REC* item;
  char        name[128];

  item = slot_item(row);
  kept_item_name(item,NAME_WIDTH,name);
  draw_slot_name(menu,row,line,name,item_color(item));
  }

/* The name a gear set keeps for an inventory row: the hands, necks, shield, armor, rings, cloak, boots and helm */
static char* set_item(GEAR_SET* set,Int32 row) {
  switch (row) {
    case I_RHND: return set->main_hand;
    case I_LHND: return set->off_hand;
    case I_NCK1: return set->neck_1;
    case I_NCK2: return set->neck_2;
    case I_SHLD: return set->shield;
    case I_ARMR: return set->armor;
    case I_RNG1: return set->ring_l1;
    case I_RNG2: return set->ring_l2;
    case I_RNG3: return set->ring_r1;
    case I_RNG4: return set->ring_r2;
    case I_ROBE: return set->cloak;
    case I_BOOT: return set->boots;
    case I_HELM: return set->helm;
    }

  return "";
  }

static Int32 set_count(GEAR_SET* set) {
  Int32 row;
  Int32 count;

  count = 0;
  for (row=1; row<NUM_SLOTS; row++)
    if (set_item(set,row)[0] != 0) count++;

  return count;
  }

/* The item named, in the main hand or wherever load_gear() looks for it, or NULL */
static OBJECT_REC* carried_item(char* name) {
  if (player.invent[I_RHND].otype != O_NONE && strcmp(name,player.invent[I_RHND].name) == 0)
    return &(player.invent[I_RHND]);

  return find_gear(name);
  }

/* A row as the gear set menu->screen keeps it: the item in its colour where you carry it, dark grey where you do not */
static void draw_set_slot(MENU* menu,Int32 row,Int32 line) {
  OBJECT_REC* item;
  char*       kept;
  char        name[128];

  kept = set_item(menu->screen,row);
  item = (kept[0] != 0) ? carried_item(kept) : NULL;

  if (item != NULL) {
    kept_item_name(item,NAME_WIDTH,name);
    draw_slot_name(menu,row,line,name,item_color(item));
    }
  else draw_slot_name(menu,row,line,kept,8);
  }

static void draw_gear_set(MENU* menu,Int32 row,Int32 line) {
  char  label[8];
  char  kept[16];
  Int32 count;

  sprintf(label,"Set %d",row);
  count = set_count(&gear_manager[row]);
  if (count == 0) strcpy(kept,"Empty");
    else sprintf(kept,"%d item%s",count,(count == 1) ? "" : "s");

  draw_menu_text_in(menu,line,label,kept,(count > 0) ? menu->color : 8);
  }

static void inventory_show_screen(MENU* menu) {
  Int32 i;
  char label[20];
  char buffer[40];
  textmode(3);
  if (improved_interface == 'Y') {
    write_centred(1,15,"Inventory");
    draw_menu(menu);
    }
  else {
    textcolor(6);
    gotoxy(1,1);
    txt_write("<");
    textcolor(14); txt_write("ESC");
    textcolor(6); txt_write(">quit, <");
    textcolor(14); txt_write("P");
    textcolor(6); txt_write(">ack, <");
    textcolor(14); txt_write("T");
    textcolor(6); txt_write(">ake from pack, e<");
    textcolor(14); txt_write("X");
    textcolor(6); txt_write(">change, <");
    textcolor(14); txt_write("ENTER");
    textcolor(6); txt_write("> select, <");
    textcolor(14); txt_write("I");
    textcolor(6); txt_write(">nfo");
    textcolor(2);
    gotoxy(1,3);
    for (i=0; i<NUM_SLOTS; i++) {
      slot_label(i,label);
      if (i == 0) sprintf(buffer,"       %s",label);
        else sprintf(buffer,"    %c) %s",'a' + i - 1,label);
      txt_writeln(buffer);
      }
    show_inventory(&in_air);
    }
  textcolor(14); gotoxy(5,24);
  sprintf(buffer,"Pack holds %d items",pack_count());
  txt_write(buffer);
  }

/* The screen again; without the Improved Interface preference the arrow marks line */
static void show_screen_at(MENU* menu,Int32 line) {
  inventory_show_screen(menu);
  if (improved_interface == 'Y') return;

  textcolor(14);
  gotoxy(1,line); txt_write("->");
  }

/* The actions for a row, each as the key that does it without the cursor. Returns the key picked, or 0. */
static char pick_action(Int32 row) {
  OBJECT_REC* item;
  char*       labels[5];
  char        keys[5];
  Int32       count;
  Int32       picked;

  item = slot_item(row);
  count = 0;

  if (item->otype != O_NONE) {
    labels[count] = "Choose";
    keys[count++] = 13;
    labels[count] = "Put in Pack";
    keys[count++] = 'P';
    }
  if (pack_count() > 0) {
    if (row == 0) labels[count] = "Take from Pack";
      else labels[count] = (item->otype != O_NONE) ? "Swap with Pack Item" : "Equip from Pack";
    keys[count++] = 'T';
    }
  if (row > 0 && in_air.otype != O_NONE && inv_valid(&in_air,row) == 'Y') {
    labels[count] = (item->otype != O_NONE) ? "Swap with Ground Item" : "Equip Ground Item";
    keys[count++] = 'X';
    }
  labels[count] = "Cancel";
  keys[count++] = 0;

  picked = ask_choice(ACTION_COLUMN,ACTION_LINE,15,labels,count,0);

  return (picked >= 0) ? keys[picked] : 0;
  }

/* Equips or saves gear set n as the keys without the inventory do: ] and [ equip the whole set or its weapons, } and
   { save your gear or weapons into it. Each takes a turn, as its key does. */
static void gear_set_action(Int32 n) {
  GEAR_SET* set;
  char*     labels[5];
  char      keys[5];
  Int32     count;
  Int32     picked;

  set = &gear_manager[n];
  count = 0;

  if (set_count(set) > 0) {
    labels[count] = "Equip Set";
    keys[count++] = ']';
    }
  if (set->main_hand[0] != 0 || set->off_hand[0] != 0) {
    labels[count] = "Equip Weapons Only";
    keys[count++] = '[';
    }
  labels[count] = "Save Current Gear";
  keys[count++] = '}';
  labels[count] = "Save Current Weapons";
  keys[count++] = '{';
  labels[count] = "Cancel";
  keys[count++] = 0;

  picked = ask_choice(ACTION_COLUMN,GEAR_ACTION_LINE,15,labels,count,0);
  if (picked < 0 || keys[picked] == 0) return;

  if (keys[picked] == ']' || keys[picked] == '[') load_gear((keys[picked] == '[') ? 'Y' : 'N',n);
    else save_gear((keys[picked] == '{') ? 'Y' : 'N',n);
  takes_time = 'Y';
  }

/* The sets over the card beside the slots */
static void show_gear_sets(MENU* slots,MENU* sets) {
  draw_card_panel(NULL,slots->rows.line + 1,slots->rows.line + slots->rows.shown);
  hide_menu_keys(slots);
  textcolor(HEADING_COLOR);
  gotoxy(ACTION_COLUMN,GEAR_LINE);
  txt_write("Gear Sets");
  draw_menu(sets);
  }

/* G: the ten gear sets where the card shows, the one under the cursor shown in the slots, and ENTER equips or saves
   it. The screen is drawn again once G or ESC goes back. */
static void gear_sets(MENU* slots) {
  MENU sets = { .rows = { .count = NUM_GEAR_SETS, .shown = NUM_GEAR_SETS, .line = GEAR_LINE },
                .column = ACTION_COLUMN, .value_column = ACTION_COLUMN + 11, .color = 2, .keys = GEAR_KEYS,
                .keys_line = 25, .draw_row = draw_gear_set };
  MENU kept;
  char caption[16];
  char key;

  kept = *slots;
  kept.keys = NULL;
  kept.draw_row = draw_set_slot;

  show_gear_sets(slots,&sets);
  key = 0;
  while (key != 27 && key != 'G' && key != 'g') {
    kept.screen = &gear_manager[sets.row];
    draw_menu(&kept);
    sprintf(caption,"Gear Set %d",sets.row);
    textcolor(HEADING_COLOR);
    gotoxy(slots->column + 4,slots->rows.line);
    txt_write(caption);

    key = menu_key(&sets);
    if (key == 13) {
      gear_set_action(sets.row);
      inventory_show_screen(slots);
      show_gear_sets(slots,&sets);
      }
    }

  inventory_show_screen(slots);
  }

/* A key with the Improved Interface preference: the cursor picks the row, shown beside its card, and ENTER an action for
   it, returned as the key that does it without the cursor. Returns 0 for the keys the cursor took. */
static char inventory_key(MENU* menu,Int32* line) {
  char key;

  draw_card_panel(slot_item(menu->row),menu->rows.line + 1,menu->rows.line + menu->rows.shown);
  key = menu_key(menu);
  *line = menu->row + 3;

  if (key == 13) return pick_action(menu->row);
  if (key == 'G' || key == 'g') gear_sets(menu);
  if (key == 27 || (key != 0 && strchr(ACTION_KEYS,key) != NULL)) return key;

  return 0;
  }

OBJECT_REC* inventory(OBJECT_REC* it) {
  char key;
  Int32  line;
  char flag;
  Int32  i,j;
  MENU   menu = { .rows = { .count = NUM_SLOTS, .shown = NUM_SLOTS, .line = 2 }, .column = 2, .value_column = 23,
                  .color = 2, .keys = INVENTORY_KEYS, .keys_line = 25, .draw_row = draw_slot };

  takes_time = 'N';
  if (it != NULL) pick_up_item(it,'g');
  in_air.otype = O_NONE;
  if (it != NULL) in_air = *it;
    else in_air.otype = O_NONE;
  if (it != NULL && is_quest_treasure(it)) {
    player.quests[player.dungeon_number] = 'Y';
    in_air.flags |= OF_IDENT;
    }
  it = NULL;
  item_to_use = NULL;
  line = 3;
  show_screen_at(&menu,line);
  flag = ' ';
  while (flag == ' ') {
    key = (improved_interface == 'Y') ? inventory_key(&menu,&line) : inkey();
    if (key == ' ' || key == '0') {
      textcolor(14);
      gotoxy(1,line); txt_write("  ");
      line = 3;
      gotoxy(1,line); txt_write("->");
      }
    if (key == '2' || key == '.' || key == '>') {
      textcolor(14);
      gotoxy(1,line); txt_write("  ");
      line++;
      if (line > 22) line = 3;
      gotoxy(1,line); txt_write("->");
      }
    if (key == '8' || key == ',' || key == '<') {
      textcolor(14);
      gotoxy(1,line); txt_write("  ");
      line--;
      if (line < 3) line = 22;
      gotoxy(1,line); txt_write("->");
      }
    if (key == 'V' || key == 'v') {
      unpack(line - 3);
      show_screen_at(&menu,line);
      }
    if (key == 'P') {
      pack_item(&in_air,line);
      show_screen_at(&menu,line);
      takes_time = 'Y';
      }
    if (key == 'T') {
      unpack(line - 3);
      show_screen_at(&menu,line);
      }
    if (key == 13) {
      if (line > 3) {
        item_to_use = &(player.invent[line -3 ]);
        use_from_ground = 'N';
        }
      else {
        item_to_use = &in_air;
        use_from_ground = 'Y';
        }
      flag = '$';
      can_drop(player.x,player.y,'Y');
      }
    if (key >= 'a' && key <= 's') {
      textcolor(14);
      gotoxy(1,line); txt_write("  ");
      line = key - 93;
      gotoxy(1,line); txt_write("->");
      }
    if ((key == 'x' || key == 'X') && line > 3) {
      do_exchange(&in_air,line);
      if (improved_interface == 'Y') show_screen_at(&menu,line);
      }
    if (key == 'Q') flag = '*';
    if (key == 'I') {
      if (line > 3) data_card(&(player.invent[line-3]));
        else data_card(&in_air);
      show_screen_at(&menu,line);
      }
    if (key == 27) {
      flag = '*';
      item_to_use = NULL;
      can_drop(player.x,player.y,'Y');
      }
    if (flag != ' ' && (drop_x < 0 || drop_y < 0)) flag = ' ';
    }

  check_level_legacy();
  player_attack_defense();
  if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();
  if (in_air.otype != O_NONE && drop_x > 0 && drop_y > 0) {
    i = find_item_slot();
    if (i != -1) {
      player.map_objects[i] = in_air;
      player.map[drop_x][drop_y] = i+1000;
      show_spot(drop_x,drop_y);
      show_player();

      /* Slot i lies at drop_x,drop_y, which need not be the player's own square */
      if (use_from_ground == 'Y') item_to_use = &(player.map_objects[i]);
      }
    } else {
    }

  i = 0;
  while (trophies[i].criteria_type < 999) {
    if (trophies[i].criteria_type == TC_EQUIP) {
      flag = '*';
      for (j=0; j<7; j++)
        if (trophies[i].control[j] > 0 && item_equipped(trophies[i].control[j]) == 0) flag = ' ';
      if (flag == '*') stats.counts[trophies[i].control[7]]++;
      }
    i++;
    }
  adjust_abilities();
  it = item_to_use;
  return it;
  }

/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The pawn shop's and the Mech-Merchant's trades. With the Improved Interface preference their pack trades list the
   items each takes, priced, beside the selected item's card, and ENTER sells or identifies the item. */

#include "header.h"

#define TRADE_ROWS      18
#define PRICE_COLUMN    39      /* the price, and the list's "more" markers */
#define ACTION_COLUMN   48      /* the choices for an item show under its card */
#define ACTION_LINE     16
#define KEYS_LINE       24
#define GOLD_LINE       25
#define NAME_WIDTH      40      /* a name in a question, which is centred on the key tips' line */
#define LEAVE_KEYS      "(ESC) Leave"

/* What a pack trade says: its heading and key tips, the choice that makes the trade and the one that does not, and the
   line shown when the pack holds nothing it takes */
typedef struct {
  char* heading;
  char* keys;
  char* action;
  char* cancel;
  char* none;
  } TRADE_WORDS;

static TRADE_WORDS selling = {
  "Sell from Pack", "(8/2) Select   (ENTER) Sell   (ESC) Leave", "Sell", "Keep", "Your pack holds nothing to sell"
  };

static TRADE_WORDS identifying = {
  "Identify from Pack", "(8/2) Select   (ENTER) Identify   (ESC) Leave", "Identify", "Cancel",
  "Your pack holds nothing unidentified"
  };

/* Selling every item in the pack, or identifying the unidentified ones, at price's price; items[] lists the pack's
   items the trade takes */
typedef struct {
  Int32        identify;
  TRADE_WORDS* words;
  double       (*price)(OBJECT_REC* item);
  OBJECT_REC*  items[PACK_SLOTS];
  Int32        count;
  } PACK_TRADE;

/* The item leaves you for price, and the sale counts in the stats */
void sell_item(OBJECT_REC* item,double price) {
  Int32 gold;

  gold = (Int32)price;
  item->otype = O_NONE;
  add_gold(gold);

  stats.counts[STAT_ITEMS_SOLD]++;
  stats.counts[STAT_SOLD_GOLD] += gold;
  if (gold > stats.counts[STAT_LARGEST_SALE]) stats.counts[STAT_LARGEST_SALE] = gold;
  }

void identify_item(OBJECT_REC* item,double price) {
  sub_gold((Int32)price);
  item->flags |= OF_IDENT;
  if (item->effect == E_MUTA) mutable_set_name(item);
  }

static Int32 takes(PACK_TRADE* trade,OBJECT_REC* item) {
  if (item->otype == O_NONE) return 0;

  return !trade->identify || (item->flags & OF_IDENT) == 0;
  }

/* Lists the pack's items the trade takes, which change as it trades */
static void list_items(PACK_TRADE* trade) {
  Int32 i;

  trade->count = 0;
  for (i=1; i<=PACK_SLOTS; i++)
    if (takes(trade,&(player.pack[i]))) trade->items[trade->count++] = &(player.pack[i]);
  }

static void draw_trade_row(MENU* menu,Int32 row,Int32 line) {
  PACK_TRADE* trade;
  OBJECT_REC* item;
  char        name[128];
  char        price[16];

  trade = menu->screen;
  item = trade->items[row];
  kept_item_name(item,menu_label_width(menu),name);
  sprintf(price,"%6.0f",trade->price(item));
  draw_menu_named(menu,line,name,item_color(item),price);
  }

static void show_trade(PACK_TRADE* trade,MENU* menu) {
  textmode(3);
  write_centred(1,11,trade->words->heading);

  list_items(trade);
  fit_menu(menu,trade->count);
  menu->keys = (trade->count > 0) ? trade->words->keys : LEAVE_KEYS;
  draw_menu(menu);

  if (trade->count == 0) {
    textcolor(7);
    gotoxy(menu->value_column - menu_label_width(menu),menu->rows.line + 1);
    txt_write(trade->words->none);
    }

  show_gold(35,GOLD_LINE);
  }

/* Asks whether to trade the selected item at its price, then sells it, or identifies it and shows what it is */
static void trade_item(PACK_TRADE* trade,MENU* menu) {
  OBJECT_REC* item;
  double      price;
  char        name[128];
  char        before[16];
  char        after[32];

  item = trade->items[menu->row];
  price = trade->price(item);

  if (trade->identify && price > get_gold()) {
    show_menu_question(menu,15,"You do not have that much!! (press a key)");
    inkey();
    return;
    }

  kept_item_name(item,NAME_WIDTH,name);
  sprintf(before,"%s ",trade->words->action);
  sprintf(after," for %.0f Gold?",price);
  show_item_question(menu,15,before,item,name,after);
  if (ask_action(ACTION_COLUMN,ACTION_LINE,15,trade->words->action,trade->words->cancel,0) != 'Y') return;

  if (!trade->identify) {
    sell_item(item,price);
    return;
    }

  identify_item(item,price);
  show_gold(35,GOLD_LINE);
  draw_card_panel(item,menu->rows.line + 1,menu->rows.line + menu->rows.shown);
  list_item_name(item,NAME_WIDTH,name);
  show_item_question(menu,15,"It is a ",item,name," (press a key)");
  inkey();
  }

static void pack_trade(PACK_TRADE* trade) {
  MENU menu = { .rows = { .shown = TRADE_ROWS, .line = 2, .more_column = PRICE_COLUMN }, .column = 2,
                .value_column = PRICE_COLUMN, .color = 14, .keys_line = KEYS_LINE, .draw_row = draw_trade_row,
                .screen = trade };
  char key;

  show_trade(trade,&menu);
  key = 0;
  while (key != 27) {
    draw_card_panel((trade->count > 0) ? trade->items[menu.row] : NULL,menu.rows.line + 1,
                    menu.rows.line + menu.rows.shown);
    key = menu_key(&menu);
    if (key == 13 && trade->count > 0) {
      trade_item(trade,&menu);
      show_trade(trade,&menu);
      }
    }
  }

void sell_from_pack(double (*offer)(OBJECT_REC* item)) {
  PACK_TRADE trade = { .words = &selling, .price = offer };

  pack_trade(&trade);
  }

void identify_from_pack(double (*cost)(OBJECT_REC* item)) {
  PACK_TRADE trade = { .identify = 1, .words = &identifying, .price = cost };

  pack_trade(&trade);
  }

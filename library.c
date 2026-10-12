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

#define NUM_SEARCHES    3

/* Picked by 'L', 'G' and 'Q' without the Improved Interface preference */
static char* search_names[NUM_SEARCHES] = { "Search for Lore", "Search for General Information", "Leave" };

/* The key of the search picked with the cursor, or 'Q' for ESC */
static char pick_search(void) {
  Int32 row;

  row = ask_choice(10,5,14,search_names,NUM_SEARCHES,0);

  return (row >= 0) ? "LGQ"[row] : 'Q';
  }

static void library_display_msg(Int32 msg,char ext) {
  MESSAGE general;
  general = (ext == 'L') ? lore[msg] : gen[msg];

  /* Without the Library Text change the plain message is enciphered, as if the tables were still stored encoded */
  if (change_library_text != 'Y') decode(general.message);
  scroll_text(general.message);
  } /* void library_display_msg(Int32 msg, char* ext) */

static void show_top(char* name) {
  char buffer[80];
  textmode(3);
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);
  textcolor(14);
  if (improved_interface != 'Y') {
    gotoxy(10,5);
    txt_write("Search for <L>ore, or <G>eneral Information ? (Q to quit)");
    }
  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  if (improved_interface == 'Y') draw_menu_keys(MENU_KEYS,25);
  } /* void show_top()() */

void library(char* name) {
  char flag;
  char search[33];
  Int32 msg;
  char key;

  flag = ' ';
  while (flag == ' ') {
    show_top(name);
    key = (improved_interface == 'Y') ? pick_search() : inkey();
    if (key == 'Q' || key == 'q') flag = '*';
    if (key == 'L' || key == 'l') {
      if (improved_interface == 'Y') key = ask_action(10,7,3,"Pay 500 gold to search","Cancel",0);
      else {
        gotoxy(10,7); textcolor(3);
        txt_write("Cost of Lore search is 500 Gold Pieces, Continue Search ?");
        key = inkey();
        }
      if (key == 'Y' || key == 'y') {
        if (get_gold()<500) {
          gotoxy(10,9); textcolor(12);
          txt_write("You do not have enough gold!!");
          delay(1000);
          }
        else {
          sub_gold(500);
          strcpy(search,"");
          while (strlen(search)<5) {
            gotoxy(10,9); textcolor(9);
            txt_write("Enter Search String (5 chars min.) ? ");
            strcpy(search,"");
            readln(search);
            }
          msg = find_lore(search);
          if (msg == 0) {
            gotoxy(10,9); textcolor(12);
            txt_write("You have all the Knowledge on that subject        ");
            delay(1000);
            }
          else {
            msg = found_row(msg);
            player.lore[msg] = -1;
            library_display_msg(msg,'L');
            }
          }
        }
      }
    else if (key == 'G' || key == 'g') {
      if (improved_interface == 'Y') key = ask_action(10,7,3,"Pay 100 gold to search","Cancel",0);
      else {
        gotoxy(10,7); textcolor(3);
        txt_write("Cost of General search is 100 Gold Pieces, Continue Search ?");
        key = inkey();
        }
      if (key == 'Y' || key == 'y') {
        if (get_gold()<100) {
          gotoxy(10,9); textcolor(12);
          txt_write("You do not have enough gold!!");
          delay(1000);
          }
        else {
          strcpy(search,"");
          sub_gold(100);
          while (strlen(search)<5) {
            gotoxy(10,9); textcolor(9);
            txt_write("Enter Search String (5 chars min.) ? ");
            strcpy(search,"");
            readln(search);
            }
          msg = find_gen(search);
          if (msg == 0) {
            gotoxy(10,9); textcolor(12);
            txt_write("You have all the Knowledge on that subject        ");
            delay(1000);
            }
          else {
            msg = found_row(msg);
            player.gen[msg] = -1;
            library_display_msg(msg,'G');
            }
          }
        }
      }
    }
}



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

/* The spell being typed at the "Spell ?" prompt */
typedef struct {
  Int32 spell_num;
  Int32 newone;
  char  spell_name[33];
  } SPELL_ENTRY;

static void match_spell(SPELL_ENTRY* entry) {
  Int32 i;
  Int32 count;
  Int32 found = 0;
  byte key;
  char buffer[80];
  count = 0;
  entry->newone = 0;
  for (i=1; i<=num_spells; i++)
    if (has_spell(i) != 0)
      if (_strnicmp(entry->spell_name,spells[i].name,strlen(entry->spell_name))  == 0) {
        count++;
        found = i;
        }
  if (count != 1) entry->newone = 0;
  else {
    clear_prompt();
    sprintf(buffer,"Spell ? %s_",spells[found].name);
    promptNS(buffer);
    key = inkey();
    if (key == 8) {
      clear_prompt();
      sprintf(buffer,"Spell ? %s_",entry->spell_name);
      promptNS(buffer);
      }
    if (key == 13) {
      entry->newone = found;
      clear_prompt();
      sprintf(buffer,"Spell ? %s ",spells[found].name);
      promptNS(buffer);
      }
    if (key == KY_F1) strcpy(player.fspell[1],spells[found].name);
    if (key == KY_F2) strcpy(player.fspell[2],spells[found].name);
    if (key == KY_F3) strcpy(player.fspell[3],spells[found].name);
    if (key == KY_F4) strcpy(player.fspell[4],spells[found].name);
    if (key == KY_F5) strcpy(player.fspell[5],spells[found].name);
    if (key == KY_F6) strcpy(player.fspell[6],spells[found].name);
    if (key == KY_F7) strcpy(player.fspell[7],spells[found].name);
    if (key == KY_F8) strcpy(player.fspell[8],spells[found].name);
    if (key == KY_F9) strcpy(player.fspell[9],spells[found].name);
    if (key == KY_F10) strcpy(player.fspell[10],spells[found].name);
    if (key == KY_F11) strcpy(player.fspell[11],spells[found].name);
    if (key == KY_F12) strcpy(player.fspell[12],spells[found].name);
    }
  }

static void show_list(void) {
  Int32  i;
  Int32  col,row;
  char buffer[80];
  textmode(3);
  col = 1;
  row = 3;
  gotoxy(1,2); textcolor(2);
  txt_write("Lvl Mana Name");
  gotoxy(40,2);
  txt_write("Lvl Mana Name");
  textcolor(14);
  for (i=1; i<=num_spells; i++)
    if (has_spell(i) != 0) {
      gotoxy(col,row);
      sprintf(buffer,"%3d %4d %s",spells[i].level,mana_cost(spells[i].mana),
              spells[i].name);
      txt_write(buffer);
      row++;
      if (row == 25) {
        row = 3;
        col = 40;
        }
      }
  inkey();
  graphmode();
  show_map();
  show_sm_map();
  show_side();
  show_player();
  }

static Int32 read_spell_name(SPELL_ENTRY* entry,Int32 column,Int32 search) {
  byte key;
  char buffer[80];
//  Int32 first;

  entry->newone = 0;
//  first = column;
  textcolor(15);
  strcpy(entry->spell_name,"");
  key = 0;
  if (entry->spell_num != 0) {
    strcpy(entry->spell_name,player.fspell[entry->spell_num]);
    sprintf(buffer,"Spell ? %s_",entry->spell_name);
    promptNS(buffer);
    if (search == 1) match_spell(entry);
    entry->spell_num = 0;
    key = 13;
    }
  while (key != 13) {
    if (entry->spell_num == 0) key = inkey();
      else entry->spell_num = 0;
    if (key == '?') {
      show_list();
      key = 3;
      clear_prompt();
      sprintf(buffer,"Spell ? %s_",entry->spell_name);
      promptNS(buffer);
      }
    if (key > 31) {
      if (key == KY_F1) strcpy(entry->spell_name,player.fspell[1]);
      if (key == KY_F2) strcpy(entry->spell_name,player.fspell[2]);
      if (key == KY_F3) strcpy(entry->spell_name,player.fspell[3]);
      if (key == KY_F4) strcpy(entry->spell_name,player.fspell[4]);
      if (key == KY_F5) strcpy(entry->spell_name,player.fspell[5]);
      if (key == KY_F6) strcpy(entry->spell_name,player.fspell[6]);
      if (key == KY_F7) strcpy(entry->spell_name,player.fspell[7]);
      if (key == KY_F8) strcpy(entry->spell_name,player.fspell[8]);
      if (key == KY_F9) strcpy(entry->spell_name,player.fspell[9]);
      if (key == KY_F10) strcpy(entry->spell_name,player.fspell[10]);
      if (key == KY_F11) strcpy(entry->spell_name,player.fspell[11]);
      if (key == KY_F12) strcpy(entry->spell_name,player.fspell[12]);
      if (key < 127) {
        entry->spell_name[strlen(entry->spell_name) + 1] = 0;
        entry->spell_name[strlen(entry->spell_name)] = key;
        sprintf(buffer,"Spell ? %s_",entry->spell_name);
        promptNS(buffer);
        }
      if (search == 1) match_spell(entry);
      if (entry->newone != 0) key = 13;
      }
    if (key == 127) key = 8;
    if (key == 8 && strlen(entry->spell_name) > 0) {
      entry->spell_name[strlen(entry->spell_name) - 1] = 0;
        clear_prompt();
        sprintf(buffer,"Spell ? %s_",entry->spell_name);
        promptNS(buffer);
      }
    if (key == 27) {
      strcpy(entry->spell_name,"");
      entry->newone = 0;
      key = 13;
      }
    }
  return entry->newone;
  }

Int32 castspell(Int32 spell_num) {
  SPELL_ENTRY entry;
  char        flag;
  Int32       newone;
//  Int32  allign;
//  Int32  level;

  entry.spell_num = spell_num;
  entry.newone = 0;
  strcpy(entry.spell_name,"");
//  allign = classes[player.class-1].spell_group;
//  level = (player.level - classes[player.class-1].becomes_caster) + 1;
  flag = ' ';
  while (flag == ' ') {
    clear_prompt();
    prompt("Spell ? _");
    newone = read_spell_name(&entry,22,1);
    flag = '*';
    }
  if (newone != 0) strcpy(player.fspell[1],spells[newone].name);
  clear_prompt();
  return newone;
  }


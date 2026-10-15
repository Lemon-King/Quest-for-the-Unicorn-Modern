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

Int32 get_spell_name(Int32 column,Int32 search,Int32 level,Int32 allign) {
  char spell_name[33];
  char buffer[80];
  Int32  first;
  Int32  newone;
  byte key;
  first = column;
  textcolor(15);
  strcpy(spell_name,"");
  newone = 0;
  key = ' ';
  while (key != 13) {
    key = inkey();
    if (key > 31) {
      if (key == KY_F1) strcpy(spell_name,player.fspell[1]);
      if (key == KY_F2) strcpy(spell_name,player.fspell[2]);
      if (key == KY_F3) strcpy(spell_name,player.fspell[3]);
      if (key == KY_F4) strcpy(spell_name,player.fspell[4]);
      if (key == KY_F5) strcpy(spell_name,player.fspell[5]);
      if (key == KY_F6) strcpy(spell_name,player.fspell[6]);
      if (key == KY_F7) strcpy(spell_name,player.fspell[7]);
      if (key == KY_F8) strcpy(spell_name,player.fspell[8]);
      if (key == KY_F9) strcpy(spell_name,player.fspell[9]);
      if (key == KY_F10) strcpy(spell_name,player.fspell[10]);
      if (key == KY_F11) strcpy(spell_name,player.fspell[11]);
      if (key == KY_F12) strcpy(spell_name,player.fspell[12]);
      if (key < 127 && strlen(spell_name) < sizeof(spell_name) - 1) {
        buffer[0] = key;
        buffer[1] = 0;
        gotoxy(column,1); txt_write(buffer); txt_write("_");
        spell_name[strlen(spell_name)+1] = 0;
        spell_name[strlen(spell_name)] = key;
        column++;
        }
      if (search == 1) newone = check_spell_1(spell_name,level,allign,first);
      if (newone != 0) key = 13;
      }
    if (key == 8 && strlen(spell_name) > 0) {
      gotoxy(column-1,1); txt_write(" "); gotoxy(column-1,1);
      column--;
      spell_name[strlen(spell_name)-1] = 0;
      }
    }
  return newone;
  }


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

Int32 check_spell_1(char* spell_name,Int32 level,Int32 allign,Int32 first) {
  Int32 i;
  Int32 count;
  Int32 found = 0;
  Int32 newone;
  byte key;
  count = 0;
  newone = 0;
  for (i=1; i<=num_spells; i++)
    if (spells[i].level <= level && has_spell(i) == 0 &&
        spells[i].allign == allign) {
      if (strncasecmp(spell_name,spells[i].name,strlen(spell_name)) == 0) {
        count++;
        found = i;
        }
      }
  if (count != 1) newone = 0;
  else {
    gotoxy(first,1); txt_write(spells[found].name);
    key = inkey();
    if (key == 8) {
      gotoxy(first,1); txt_write("                                        ");
      gotoxy(first,1); txt_write(spell_name); txt_write("_");
      }
    if (key == 13) newone = found;
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
  return newone;
  }


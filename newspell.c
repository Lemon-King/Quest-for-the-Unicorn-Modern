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

void new_spell() {
  Int32 i;
  Int32 col,row;
  char flag;
  Int32  newone;
  Int32  allign = 0;
  Int32  level = 0;
  Int32  no_spells;
  char buffer[80];
  Int32 column;
  no_spells = 0;
  allign = classes[player.class-1].spell_group;
  level = (player.level - classes[player.class-1].becomes_caster) + 1;
  textmode(3);
  col = 1;
  row = 3;
  gotoxy(1,2); textcolor(2); 
  txt_write("Lvl Mana Name");
  gotoxy(40,2);
  txt_write("Lvl Mana Name"); 
  textcolor(14);
  for (i=1; i<=num_spells; i++)
    if (spells[i].level <= level && has_spell(i) == 0 &&
        spells[i].allign == allign) {
      no_spells++;
      gotoxy(col,row);
      sprintf(buffer,"%3d %4d %s",spells[i].level,mana_cost(spells[i].mana),
              spells[i].name);
      txt_write(buffer);
      row++;
      if (row == 25) {
        row = 2;
        col = 40;
        }
      }
  if (no_spells != 0) { 
    flag = ' '; 
    while (flag == ' ') {
      gotoxy(1,1); textcolor(11); column = 2 + write_question("Select which spell"); txt_write("          ");
      gotoxy(column,1); txt_write("_");
      newone = get_spell_name(column,1,level,allign);
      if (newone != 0) flag = '*';
      }
    i = 1;
    while (spellbook[i] != 0) i++;
    spellbook[i] = newone;
    }
  }


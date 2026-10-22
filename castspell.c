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

/* A spell picked from the Tab Menu's list, which the next cast takes without asking; 0 for none */
static Int32 picked_spell = 0;

/* The spells you know, in the order ? lists them, into list. Returns how many. */
Int32 known_spells(SPELLBOOK list) {
  Int32 i;
  Int32 count;

  count = 0;
  for (i=1; i<=num_spells; i++)
    if (has_spell(i) != 0) list[count++] = i;

  return count;
  }

Int32 knows_a_spell(void) {
  SPELLBOOK known;

  return (known_spells(known) > 0);
  }

void pick_spell(Int32 spell) {
  picked_spell = spell;
  }

/* The one spell you know whose name begins with name; 0 when none or several do */
static Int32 matching_spell(char* name) {
  Int32 i;
  Int32 count;
  Int32 found;

  count = 0;
  found = 0;
  for (i=1; i<=num_spells; i++)
    if (has_spell(i) != 0 && _strnicmp(name,spells[i].name,strlen(name)) == 0) {
      count++;
      found = i;
      }

  return (count == 1) ? found : 0;
  }

/* The spell you know called name, in any case; 0 for none */
static Int32 find_spell(char* name) {
  Int32 i;

  for (i=1; i<=num_spells; i++)
    if (has_spell(i) != 0 && _stricmp(name,spells[i].name) == 0) return i;

  return 0;
  }

static Int32 complete_spell(TEXT_FIELD* field,char* word) {
  Int32 spell;

  spell = matching_spell(field->text);
  if (spell == 0) return 0;

  strcpy(word,spells[spell].name);
  return 1;
  }

static void show_list(void) {
  SPELLBOOK known;
  Int32  count;
  Int32  i;
  Int32  col,row;
  char buffer[80];
  count = known_spells(known);
  textmode(3);
  col = 1;
  row = 3;
  gotoxy(1,2); textcolor(2);
  txt_write("Lvl Mana Name");
  gotoxy(40,2);
  txt_write("Lvl Mana Name");
  textcolor(14);
  for (i=0; i<count; i++) {
    gotoxy(col,row);
    sprintf(buffer,"%3d %4d %s",spells[known[i]].level,mana_cost(spells[known[i]].mana),
            spells[known[i]].name);
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

/* ? lists the spells you know. F1-F12 keep the one spell the name begins in their slot, or bring back the spell kept
   there. */
static Int32 spell_key(TEXT_FIELD* field,Int32 key) {
  Int32 slot;
  Int32 spell;

  if (key == '?') {
    show_list();
    return 0;
    }

  slot = fkey_number(key);
  if (slot == 0) return key;

  spell = (field->text[0] != 0) ? matching_spell(field->text) : 0;
  if (spell != 0) strcpy(player.fspell[slot],spells[spell].name);
    else sprintf(field->text,"%.*s",field->length,player.fspell[slot]);

  return 0;
  }

/* The spell typed at the prompt, which an F-key's slot fills when not 0; 0 for none, and for an empty slot */
static Int32 read_spell(Int32 slot) {
  TEXT_FIELD field;
  char       name[33];

  strcpy(name,(slot != 0) ? player.fspell[slot] : "");
  if (slot != 0 && name[0] == 0) return 0;

  open_log_field(&field,name,sizeof(name),"Spell",10);
  field.cancel = 1;
  field.complete = complete_spell;
  field.key = spell_key;
  if (!read_field(&field)) return 0;

  return find_spell(name);
  }

Int32 castspell(Int32 spell_num) {
  Int32 newone;

  if (picked_spell != 0) {
    newone = picked_spell;
    picked_spell = 0;
    }
  else newone = read_spell(spell_num);

  if (newone != 0) strcpy(player.fspell[1],spells[newone].name);
  return newone;
  }


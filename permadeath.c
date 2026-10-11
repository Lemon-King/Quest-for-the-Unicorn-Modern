/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 * Permadeath, a rule chosen when the character is created; Casual never has it. While a character with permadeath is
 * played, its save is held as <name>.bak, which every exit removes, so a character that dies leaves no save to reload.
 */

#include "header.h"

Int32 has_permadeath() {
  return (permadeath == 'Y' && player.effect[PE_HEROIC] >= 0);
  }

/* Moves <name>.sav to <name>.bak, in place of the save held before it */
void hold_save(char* name) {
#ifdef HC
  char filename[sizeof(backupname)];

  if (!has_permadeath()) return;

  strcpy(filename,name);
  strcat(filename,".sav");

  remove(backupname);
  strcpy(backupname,name);
  strcat(backupname,".bak");

  remove(backupname);
  rename(filename,backupname);
#endif
  }

/* For a character turned Casual: puts the held save back as <name>.sav */
void end_permadeath() {
#ifdef HC
  char   filename[sizeof(backupname)];
  size_t length;
#endif

  permadeath = 'N';

#ifdef HC
  length = strlen(backupname);
  if (length <= 4) return;

  strcpy(filename,backupname);
  strcpy(filename + length - 4,".sav");
  rename(backupname,filename);
#endif
  }

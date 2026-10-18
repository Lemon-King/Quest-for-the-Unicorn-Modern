/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Words of Power and the Talning rod's functions: which the character has read of, and the one a typed word says. A
   word is known once the lore that names it in quotes, as "Mintaal", has been read. */

#include "header.h"

/* The Easy Words preference, which needs Improved Interface */
Int32 easy_words_on(void) {
  return (easy_words == 'Y' && improved_interface == 'Y');
  }

static Int32 is_known(WORD_OF_POWER* word) {
  char  quoted[40];
  Int32 i;

  sprintf(quoted,"\"%.36s\"",word->word);
  for (i=0; lore[i].link != 999; i++)
    if (player.lore[i] != 0 && strstr(lore[i].message,quoted) != NULL) return 1;

  return 0;
  }

/* The words of book the character knows, as their places in it. Returns how many. */
Int32 known_words(WORD_BOOK* book,Int32* list) {
  Int32 count;
  Int32 i;

  count = 0;
  for (i=0; i<book->count && count<MAX_WORDS; i++)
    if (is_known(&book->words[i])) list[count++] = i;

  return count;
  }

/* The word of book that text says, in any case; NULL for none */
WORD_OF_POWER* find_word(WORD_BOOK* book,char* text) {
  Int32 i;

  for (i=0; i<book->count; i++)
    if (_stricmp(text,book->words[i].word) == 0) return &book->words[i];

  return NULL;
  }

/* A TEXT_FIELD's complete function for the WORD_BOOK in its data: the one known word the text begins */
Int32 complete_word(TEXT_FIELD* field,char* word) {
  WORD_BOOK* book;
  Int32      length;
  Int32      count;
  Int32      found;
  Int32      i;

  book = field->data;
  length = (Int32)strlen(field->text);
  count = 0;
  found = 0;
  for (i=0; i<book->count; i++)
    if (_strnicmp(field->text,book->words[i].word,length) == 0 && is_known(&book->words[i])) {
      count++;
      found = i;
      }

  if (count != 1) return 0;

  strcpy(word,book->words[found].word);
  return 1;
  }

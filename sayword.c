/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Words of Power, said with w in towns and dungeons. Each has its chance, and most backfire when it fails. */

#include "header.h"

/* A word picked from the Tab Menu's list, which the next w says without asking; empty for none */
static char picked_word[33] = "";

static void word_fails(void) {
  if (easy_words_on()) msg("The word fails",12);
  }

static void word_backfires(void) {
  if (easy_words_on()) msg("The word backfires!",12);
  }

static void to_surface(void) {
  if (u_random(100) < 95) special(15,NULL);
  else {
    word_backfires();
    go_bottom();
    }
  }

static void ilital(void) {
  if (u_random(100) < 95) go_up();
  else {
    word_backfires();
    go_down();
    }
  }

static void latili(void) {
  if (u_random(100) < 95) go_down();
  else {
    word_backfires();
    go_up();
    }
  }

static void indral(void) {
  if (u_random(100) < 95) player.effect[PE_POISON] = 0;
  else {
    word_backfires();
    player.effect[PE_POISON] *= 2;
    }
  }

static void oraclal(void) {
  if (u_random(100) < 90) special(1,NULL);
    else word_fails();
  }

static void illumina(void) {
  if (u_random(100) < 95) special(6,NULL);
  else {
    word_backfires();
    player.effect[PE_LIGHT] = 0;
    }
  }

static void auraral(void) {
  if (u_random(100) < 75) special(42,NULL);
    else word_fails();
  }

static void manatal(void) {
  if (u_random(100) < 75) special(19,NULL);
  else {
    word_backfires();
    player.cmana = 0;
    }
  }

static void hurar(void) {
  if (u_random(100) < 75) special(3,NULL);
  else {
    word_backfires();
    player.effect[PE_SLEEP] += 10;
    }
  }

static void fearaway(void) {
  if (u_random(100) < 75) special(36,NULL);
  else {
    word_backfires();
    player.effect[PE_FEAR] *= 2;
    }
  }

static void blinkahl(void) {
  if (u_random(100) < 75) do_blink();
    else word_fails();
  }

static void visorae(void) {
  Int32 x,y;

  if (u_random(100) < 50) special(14,NULL);
  else {
    word_backfires();
    for (x=1; x<=100; x++)
      for (y=1; y<=100; y++) player.known[x][y] = 0;
    }

  show_screen();
  }

/* No lore names Laatnim or Latili, the first two words backwards */
static WORD_OF_POWER spoken[] = {
  { "Mintaal",  "Return to the surface",    to_surface },
  { "Laatnim",  NULL,                       to_surface },
  { "Ilital",   "Up one level",             ilital },
  { "Latili",   NULL,                       latili },
  { "Indral",   "Slow poison",              indral },
  { "Oraclal",  "Clairaudience",            oraclal },
  { "Illumina", "Light in the dark",        illumina },
  { "Auraral",  "Summon an Aerial Servant", auraral },
  { "Manatal",  "Increase mana",            manatal },
  { "Hurar",    "Speed you up",             hurar },
  { "Fearaway", "Banish fear",              fearaway },
  { "Blinkahl", "Escape your enemies",      blinkahl },
  { "Visorae",  "Reveal your surroundings", visorae }
  };

static WORD_BOOK spoken_book = { spoken, sizeof(spoken) / sizeof(spoken[0]) };

WORD_BOOK* words_of_power(void) {
  return &spoken_book;
  }

void pick_word(char* word) {
  sprintf(picked_word,"%.*s",(Int32)sizeof(picked_word) - 1,word);
  }

/* The word picked from the Tab Menu, or else the one typed; ESC backs out with Improved Interface. Returns 0 when none
   is said. */
static Int32 read_word(char* word,Int32 size) {
  TEXT_FIELD field;

  if (picked_word[0] != 0) {
    sprintf(word,"%.*s",size - 1,picked_word);
    picked_word[0] = 0;
    return 1;
    }

  strcpy(word,"");
  open_log_field(&field,word,size,"Say ? ",10);
  field.cancel = (improved_interface == 'Y');
  if (easy_words_on()) {
    field.complete = complete_word;
    field.data = &spoken_book;
    }

  return read_field(&field);
  }

/* A Word of Power rests your voice for 1000 turns; anything else costs nothing */
void say_word() {
  WORD_OF_POWER* said;
  char           word[33];

  if (!read_word(word,sizeof(word))) return;

  said = find_word(&spoken_book,word);
  if (said == NULL) {
    if (easy_words_on() && word[0] != 0) msg("That is no Word of Power",12);
    return;
    }

  word_counter = 1000;
  said->say();
  }

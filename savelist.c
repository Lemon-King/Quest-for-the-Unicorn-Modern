/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The saved games in the working directory, newest first, with each one's character. */

#include "header.h"

#include <io.h>

static void add_saved_game(SAVE_LIST* list, char* file_name, time_t saved) {
  SAVED_GAME* games;
  SAVED_GAME* game;
  Int32       length;
  length = strlen(file_name);

  if (length <= 4 || strcmp(file_name + length - 4, ".sav") != 0) return;

  /* load() names the backup <name>.bak in backupname */
  if (length >= sizeof(backupname)) return;

  if (list->count == list->capacity) {
    games = realloc(list->games, (list->capacity + 16) * sizeof(SAVED_GAME));
    if (games == NULL) return;
    list->games = games;
    list->capacity += 16;
    }

  game = &list->games[list->count];
  strcpy(game->name, file_name);
  game->name[length - 4] = 0;
  game->saved = saved;
  game->readable = load_summary(game->name, &game->summary);
  list->count++;
  }

static void read_saved_games(SAVE_LIST* list) {
  struct _finddata_t found;
  intptr_t           search;

  search = _findfirst("*.sav", &found);
  if (search == -1) return;

  do {
    if ((found.attrib & _A_SUBDIR) == 0) add_saved_game(list, found.name, found.time_write);
    } while (_findnext(search, &found) == 0);

  _findclose(search);
  }

static int newest_first(const void* a, const void* b) {
  const SAVED_GAME* first;
  const SAVED_GAME* second;
  first = a;
  second = b;

  if (first->saved > second->saved) return -1;
  if (first->saved < second->saved) return 1;
  return strcmp(first->name, second->name);
  }

void open_save_list(SAVE_LIST* list) {
  list->games = NULL;
  list->count = 0;
  list->capacity = 0;

  read_saved_games(list);
  if (list->count > 1) qsort(list->games, list->count, sizeof(SAVED_GAME), newest_first);
  }

void close_save_list(SAVE_LIST* list) {
  free(list->games);
  list->games = NULL;
  list->count = 0;
  list->capacity = 0;
  }

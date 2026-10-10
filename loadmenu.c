/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The Load Game screen: the saved games in the working directory, newest first, with each one's character. */

#include "header.h"

#include <io.h>

#define LIST_LINE        4
#define LIST_ROWS       18
#define COUNT_LINE      22
#define CURSOR_COLUMN    2
#define NAME_COLUMN      6
#define NAME_WIDTH      20
#define CLASS_COLUMN    28
#define CLASS_WIDTH     10
#define LEVEL_COLUMN    40
#define LEVEL_WIDTH      3
#define MODE_COLUMN     45
#define MODE_WIDTH       6
#define TIME_COLUMN     53
#define TIME_WIDTH      15
#define DATE_COLUMN     70
#define DATE_WIDTH      10
#define ROW_WIDTH       (DATE_COLUMN + DATE_WIDTH - CURSOR_COLUMN)

typedef struct {
  char         name[sizeof(loadname)];
  time_t       saved;
  SAVE_SUMMARY summary;
  Int32        readable;
  } SAVED_GAME;

typedef struct {
  SAVED_GAME* games;
  Int32       count;
  Int32       capacity;
  } SAVE_LIST;

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

static void open_save_list(SAVE_LIST* list) {
  list->games = NULL;
  list->count = 0;
  list->capacity = 0;

  read_saved_games(list);
  if (list->count > 1) qsort(list->games, list->count, sizeof(SAVED_GAME), newest_first);
  }

static void close_save_list(SAVE_LIST* list) {
  free(list->games);
  list->games = NULL;
  list->count = 0;
  list->capacity = 0;
  }

/* Writes value padded or cut to width, so it covers whatever the row showed before. */
static void write_field(Int32 column, Int32 line, Int32 width, char* value) {
  char text[81];
  sprintf(text, "%-*.*s", width, width, value);
  gotoxy(column, line); txt_write(text);
  }

static void draw_summary(SAVED_GAME* game, Int32 line) {
  char level[16];
  char played[32];

  if (!game->readable) {
    write_field(CLASS_COLUMN, line, TIME_COLUMN + TIME_WIDTH - CLASS_COLUMN, "Unreadable");
    return;
    }

  sprintf(level, "%*d", LEVEL_WIDTH, game->summary.level);
  convert_time(game->summary.time_played, played);

  write_field(CLASS_COLUMN, line, CLASS_WIDTH, classes[game->summary.class - 1].name);
  write_field(LEVEL_COLUMN, line, LEVEL_WIDTH, level);
  write_field(MODE_COLUMN, line, MODE_WIDTH, mode_name(game->summary.mode));
  write_field(TIME_COLUMN, line, TIME_WIDTH, played);
  }

static void draw_saved_game(SAVE_LIST* list, Int32 i, Int32 top, char selected) {
  SAVED_GAME* game;
  char        stamp[DATE_WIDTH + 1];
  struct tm*  when;
  Int32       line;
  game = &list->games[i];
  line = LIST_LINE + i - top;

  textcolor(15);
  gotoxy(CURSOR_COLUMN, line); txt_write((selected == 'Y') ? "-->" : "   ");

  textcolor((selected == 'Y') ? 14 : 2);
  write_field(NAME_COLUMN, line, NAME_WIDTH, game->name);

  textcolor(7);
  draw_summary(game, line);

  when = localtime(&game->saved);
  if (when == NULL) strcpy(stamp, "");
    else strftime(stamp, sizeof(stamp), "%Y-%m-%d", when);
  write_field(DATE_COLUMN, line, DATE_WIDTH, stamp);
  }

static void draw_page(SAVE_LIST* list, Int32 top) {
  char  text[80];
  Int32 i;

  for (i=top; i<top+LIST_ROWS; i++) {
    if (i < list->count) draw_saved_game(list, i, top, 'N');
    else {
      sprintf(text, "%*s", ROW_WIDTH, "");
      gotoxy(CURSOR_COLUMN, LIST_LINE + i - top); txt_write(text);
      }
    }

  if (list->count > LIST_ROWS) {
    sprintf(text, "Games %d-%d of %d   ", top + 1, top + LIST_ROWS, list->count);
    textcolor(8);
    gotoxy(NAME_COLUMN, COUNT_LINE); txt_write(text);
    }
  }

static void draw_load_screen(SAVE_LIST* list) {
  textmode(3);
  textcolor(12);
  gotoxy(37, 1); txt_write("Load Game");

  if (list->count == 0) {
    textcolor(2);
    gotoxy(NAME_COLUMN, LIST_LINE); txt_write("There are no saved games in this folder.");
    textcolor(3);
    gotoxy(36, 24); txt_write("(ESC) Back");
    return;
    }

  textcolor(11);
  gotoxy(NAME_COLUMN, LIST_LINE - 1); txt_write("Saved Game");
  gotoxy(CLASS_COLUMN, LIST_LINE - 1); txt_write("Class");
  gotoxy(LEVEL_COLUMN, LIST_LINE - 1); txt_write("Lvl");
  gotoxy(MODE_COLUMN, LIST_LINE - 1); txt_write("Mode");
  gotoxy(TIME_COLUMN, LIST_LINE - 1); txt_write("Time Played");
  gotoxy(DATE_COLUMN, LIST_LINE - 1); txt_write("Last Saved");

  textcolor(3);
  gotoxy(21, 24); txt_write("(8/2) Select   (ENTER) Load   (ESC) Back");
  }

/* Returns the index of the chosen game, or -1 when the player backs out. */
static Int32 choose_saved_game(SAVE_LIST* list) {
  Int32 selected;
  Int32 top;
  char  key;
  draw_load_screen(list);

  if (list->count == 0) {
    key = 0;
    while (key != 13 && key != 27) key = inkey();
    return -1;
    }

  selected = 0;
  top = 0;
  draw_page(list, top);

  key = 0;
  while (key != 13 && key != 27) {
    draw_saved_game(list, selected, top, 'Y');
    key = inkey();
    draw_saved_game(list, selected, top, 'N');

    if (key == '8') selected--;
    if (key == '2') selected++;
    if (key == '9') selected -= LIST_ROWS;
    if (key == '3') selected += LIST_ROWS;
    if (selected < 0) selected = 0;
    if (selected >= list->count) selected = list->count - 1;

    if (selected < top || selected >= top + LIST_ROWS) {
      if (selected < top) top = selected;
        else top = selected - LIST_ROWS + 1;
      draw_page(list, top);
      }
    }

  return (key == 13) ? selected : -1;
  }

/* Returns 1 with the chosen game's name in loadname, or 0 when the player backs out. */
Int32 load_menu() {
  SAVE_LIST list;
  Int32     chosen;
  open_save_list(&list);

  chosen = choose_saved_game(&list);
  if (chosen >= 0) strcpy(loadname, list.games[chosen].name);

  close_save_list(&list);
  return (chosen >= 0) ? 1 : 0;
  }

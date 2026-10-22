/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* A line of typed text, in the log after its label or on the text grid. It keeps every key typed into it, ESC backs out
   where the field allows it, and TAB or ENTER takes the word its complete function offers, shown in grey after the
   text. */

#include "header.h"

#define OFFERED_COLOR   CLR_DARKGRAY

static Int32 is_printable(Int32 key) {
  return (key >= 32 && key < 127);
  }

Int32 is_digit_key(Int32 key) {
  return (key >= '0' && key <= '9');
  }

/* A field over text, a buffer of size characters, taking at most length of them */
static void open_field(TEXT_FIELD* field,char* text,Int32 size,Int32 length,Int32 color) {
  field->text = text;
  field->length = (size - 1 < length) ? size - 1 : length;
  field->label[0] = 0;
  field->x = 0;
  field->y = 0;
  field->color = color;
  field->cancel = 0;
  field->accepts = is_printable;
  field->complete = NULL;
  field->key = NULL;
  field->data = NULL;
  field->drawn = 0;

  if ((Int32)strlen(text) > field->length) text[field->length] = 0;
  }

/* The label asks the question, ended by question_mark(): "Say? " with Improved Interface, "Say ? " without */
void open_log_field(TEXT_FIELD* field,char* text,Int32 size,char* question,Int32 color) {
  open_field(field,text,size,TEXT_LENGTH,color);
  sprintf(field->label,"%.*s%s ",(Int32)sizeof(field->label) - 4,question,question_mark());
  }

/* Columns x to 80 hold the text and its cursor */
void open_grid_field(TEXT_FIELD* field,char* text,Int32 size,Int32 x,Int32 y,Int32 color) {
  open_field(field,text,size,(x < 80) ? 80 - x : 0,color);
  field->x = x;
  field->y = y;
  }

/* The columns the text, the rest of an offered word and the cursor share */
static Int32 field_room(TEXT_FIELD* field) {
  if (field->label[0] != 0) return log_width() - (Int32)strlen(field->label);

  return 81 - field->x;
  }

/* The newest log line, or with drawn 0 a new one */
static void draw_in_log(TEXT_FIELD* field,char* shown,char* rest,char* cursor) {
  char  line[2 * TEXT_LENGTH + 40];
  Int32 at;

  sprintf(line,"%s%s%s%s",field->label,shown,rest,cursor);
  at = (Int32)(strlen(field->label) + strlen(shown));

  if (rest[0] != 0) msg_split(line,(field->drawn > 0) ? -field->color : field->color,at,OFFERED_COLOR);
    else msg_split(line,(field->drawn > 0) ? -field->color : field->color,0,0);

  field->drawn = (Int32)strlen(line);
  }

/* Spaces cover what the last drawing left beyond this one */
static void draw_on_grid(TEXT_FIELD* field,char* shown,char* rest,char* cursor) {
  char  blank[TEXT_LENGTH + 2];
  Int32 columns;

  gotoxy(field->x,field->y);
  textcolor(field->color);
  txt_write(shown);
  textcolor(OFFERED_COLOR);
  txt_write(rest);
  textcolor(field->color);
  txt_write(cursor);

  columns = (Int32)(strlen(shown) + strlen(rest) + strlen(cursor));
  if (field->drawn > columns) {
    sprintf(blank,"%*s",field->drawn - columns,"");
    txt_write(blank);
    }

  field->drawn = columns;
  }

/* The text, or as much of its end as fits, then the rest of word in grey, then the cursor */
static void draw_field(TEXT_FIELD* field,char* word,char* cursor) {
  char  shown[TEXT_LENGTH + 1];
  char  rest[TEXT_LENGTH + 1];
  Int32 room;
  Int32 length;

  room = field_room(field) - (Int32)strlen(cursor);
  if (room < 0) room = 0;

  length = (Int32)strlen(field->text);
  strcpy(shown,field->text + ((length > room) ? length - room : 0));

  if ((Int32)strlen(word) > length) sprintf(rest,"%.*s",room - (Int32)strlen(shown),word + length);
    else strcpy(rest,"");

  if (field->label[0] != 0) draw_in_log(field,shown,rest,cursor);
    else draw_on_grid(field,shown,rest,cursor);
  }

/* The word complete offers for the text, into word; empty when it offers none */
static void offered_word(TEXT_FIELD* field,char* word) {
  word[0] = 0;
  if (field->complete == NULL || field->text[0] == 0) return;

  if (!field->complete(field,word)) word[0] = 0;
  }

static void take_word(TEXT_FIELD* field) {
  char word[TEXT_LENGTH + 1];

  offered_word(field,word);
  if (word[0] != 0) sprintf(field->text,"%.*s",field->length,word);
  }

/* Reads keys into the field until ENTER, or ESC where it allows that. The first key throws away the keys typed before
   the field opened, as inkey() does. Returns 1 for ENTER. */
Int32 read_field(TEXT_FIELD* field) {
  char  word[TEXT_LENGTH + 1];
  Int32 first;
  Int32 length;
  Int32 key;

  first = 1;
  key = 0;
  while (key != 13 && key != 27) {
    offered_word(field,word);
    draw_field(field,word,"_");

    key = (unsigned char)(first ? inkey() : typed_key());
    first = 0;

    if (field->key != NULL) key = field->key(field,key);
    if (key == 27 && !field->cancel) key = 0;

    length = (Int32)strlen(field->text);
    if ((key == 8 || key == 127) && length > 0) field->text[length - 1] = 0;
    if (key == 9 || key == 13) take_word(field);
    if (field->accepts(key) && length < field->length) {
      field->text[length] = (char)key;
      field->text[length + 1] = 0;
      }
    }

  draw_field(field,"","");

  return (key == 13);
  }

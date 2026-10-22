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
#include <stdarg.h>

/* Characters on a log line: the box's whole width with Improved Combat Logs */
Int32 log_width(void) {
  return (improved_combat_logs == 'Y') ? WIDE_LOG_WIDTH : LOG_WIDTH;
  }

static void copy_line(Int32 to,Int32 from) {
  strcpy(messages[to],messages[from]);
  msg_clr[to] = msg_clr[from];
  msg_name_clr[to] = msg_name_clr[from];
  msg_name_at[to] = msg_name_at[from];
  }

static void blank_line(Int32 line) {
  strcpy(messages[line],"");
  msg_clr[line] = 15;
  msg_name_clr[line] = 0;
  msg_name_at[line] = 0;
  }

void scroll_msg() {
  Int32 i;
  for (i=0; i<NUM_MSG-1; i++) copy_line(i,i+1);
  }

/* The log shows count lines, the newest still at the bottom: it loses its oldest lines, or gains blank ones above */
void set_log_lines(Int32 count) {
  Int32 shift;
  Int32 i;

  shift = count - NUM_MSG;

  if (shift < 0)
    for (i=0; i<count; i++) copy_line(i,i - shift);

  if (shift > 0)
    for (i=count-1; i>=0; i--) {
      if (i >= shift) copy_line(i,i - shift);
        else blank_line(i);
      }

  NUM_MSG = count;
  }

/* The newest line's colours; name_at, where the item name starts, lies before the line when the name began on an
   earlier one */
static void color_line(Int32 color,Int32 name_color,Int32 name_at) {
  msg_clr[NUM_MSG-1] = color;
  msg_name_clr[NUM_MSG-1] = name_color;
  msg_name_at[NUM_MSG-1] = (name_at > 0) ? name_at : 0;
  }

/* message in color, wrapped onto the log's lines; from name_start on in name_color, or none when that is 0 */
static void add_message(char* message,Int32 color,Int32 name_start,Int32 name_color) {
  char buffer[256];
  char *pChar;
  char *next;
  Int32 width;
  strcpy(buffer,message);
  pChar = buffer;
  width = log_width();
  if (color >= 0) {
    scroll_msg();
    }
  color = abs(color);
  while ((Int32)strlen(pChar) > width) {
    next = pChar + width;
    while (next > pChar && *next != ' ') next--;

    color_line(color,name_color,name_start - (Int32)(pChar - buffer));

    /* A word longer than a line breaks at the line's end */
    if (next == pChar) {
      strncpy(messages[NUM_MSG-1],pChar,width);
      messages[NUM_MSG-1][width] = 0;
      pChar += width;
      }
    else {
      *next = 0;
      strcpy(messages[NUM_MSG-1],pChar);
      pChar = next + 1;
      }

    if (color >= 0) scroll_msg();
    }
  color_line(color,name_color,name_start - (Int32)(pChar - buffer));
  strcpy(messages[NUM_MSG-1],pChar);
  show_messages();
  }

void msg(char* message,Int32 color) {
  add_message(message,color,0,0);
  }

/* msg() with the line made by printf's format */
void msgf(Int32 color,char* format,...) {
  char message[256];
  va_list args;

  va_start(args,format);
  vsnprintf(message,sizeof(message),format,args);
  va_end(args);

  msg(message,color);
  }

/* words, then an item's name, with only the name in its rarity colour, name_color */
void msg_named(char* words,char* name,Int32 color,Int32 name_color) {
  char message[256];

  sprintf(message,"%s %.200s",words,name);
  add_message(message,color,(Int32)strlen(words) + 1,name_color);
  }

/* message in color, and from column at on in second_color, 0 for none; with color below 0 it replaces the newest line */
void msg_split(char* message,Int32 color,Int32 at,Int32 second_color) {
  add_message(message,color,at,second_color);
  }


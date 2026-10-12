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

void scroll_msg() {
  Int32 i;
  for (i=0; i<NUM_MSG-1; i++) {
    strcpy(messages[i],messages[i+1]);
    msg_clr[i] = msg_clr[i+1];
    msg_name_clr[i] = msg_name_clr[i+1];
    msg_name_at[i] = msg_name_at[i+1];
    }
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
  strcpy(buffer,message);
  pChar = buffer;
  if (color >= 0) {
    scroll_msg();
    }
  color = abs(color);
  while (strlen(pChar) > 21) {
    next = pChar + 21;
    while (next > pChar && *next != ' ') next--;

    color_line(color,name_color,name_start - (Int32)(pChar - buffer));

    /* A word longer than a line breaks at the line's end */
    if (next == pChar) {
      strncpy(messages[NUM_MSG-1],pChar,21);
      messages[NUM_MSG-1][21] = 0;
      pChar += 21;
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

/* words, then an item's name, with only the name in its rarity colour, name_color */
void msg_named(char* words,char* name,Int32 color,Int32 name_color) {
  char message[256];

  sprintf(message,"%s %.200s",words,name);
  add_message(message,color,(Int32)strlen(words) + 1,name_color);
  }


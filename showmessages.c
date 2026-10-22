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

/* The box under the player's stats that the log, or the Tab Menu, fills. Without the bottom strip it holds the detail
   line too. */
void clear_messages(void) {
  Int32 bottom;

  bottom = has_bottom_strip() ? 100 + NUM_MSG * MESSAGE_HEIGHT : detail_line_bottom();
  setfillstyle(1,0);
  bar(screen_width-220,94,screen_width,bottom);
  }

/* Each line in its colour, and an item name on it in the name's own */
void show_messages() {
  Int32 i;
  Int32 at;
  Int32 width;
  char  words[WIDE_LOG_WIDTH + 2];
  clear_messages();
  width = log_width();
  for (i=0; i<NUM_MSG; i++) {
    setcolor(msg_clr[i]);
    while ((Int32)strlen(messages[i]) < width + 1) strcat(messages[i]," ");
  if ((Int32)strlen(messages[i]) > width + 1) messages[i][width] = 0;
    at = msg_name_at[i];
    if (msg_name_clr[i] == 0 || at >= (Int32)strlen(messages[i])) {
      outtextxy(screen_width-215,110+i*12,messages[i]);
      continue;
      }

    sprintf(words,"%.*s",at,messages[i]);
    outtextxy(screen_width-215,110+i*12,words);
    setcolor(msg_name_clr[i]);
    outtextxy(screen_width-215 + at*8,110+i*12,messages[i] + at);
    }
  show_detail_line();
  }


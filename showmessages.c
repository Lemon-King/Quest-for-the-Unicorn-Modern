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

/* The box under the player's stats that the log, or the Tab Menu, fills */
void clear_messages(void) {
  setfillstyle(1,0);
  bar(screen_width-220,94,screen_width,100 + NUM_MSG * MESSAGE_HEIGHT);
  }

void show_messages() {
  Int32 i;
  clear_messages();
  for (i=0; i<NUM_MSG; i++) {
    setcolor(msg_clr[i]);
    while (strlen(messages[i]) < 22) strcat(messages[i]," ");
  if (strlen(messages[i]) > 22) messages[i][21] = 0;
    outtextxy(screen_width-215,110+i*12,messages[i]);
    }
  }


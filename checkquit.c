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

/* Asks before Q ends the character. Returns -1 when the player says yes. */
Int32 check_quit(void) {
  Int32 key;
  saveScreen();
  clrscr();
  textcolor(12); gotoxy(35,8); txt_write("WARNING!!!");
  textcolor(15); gotoxy(6,10); txt_write("<Q>uit will end your game with this character.");
  textcolor(15); gotoxy(6,11); txt_write("Are you sure (Y/N)?");
  key = ' ';
  while (key != 'Y' && key != 'N') {
    key = inkey();
    if (key == 'y') key = 'Y';
    if (key == 'n') key = 'N';
    }
  restoreScreen();
  return (key == 'Y') ? -1 : 0;
  }

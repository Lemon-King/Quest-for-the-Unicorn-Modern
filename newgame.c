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

void new_game() {
  textmode(3);
  textcolor(2);
  gotoxy(1,5);
  txt_writeln("  In ages past the unicorns roamed over much of Halkanar, now they are but");
  txt_writeln("  a distant memory and mentioned only in legend.  Many believe that the");
  txt_writeln("  unicorns were nothing but myth and never actually existed.  Others believe");
  txt_writeln("  that they did and that some tragedy had befallen them.");
  txt_writeln("");
  txt_writeln("  Will you be able to unravel the mystery of the unicorns? Will you be able");
  txt_writeln("  to find them?");
  txt_writeln("");
  txt_writeln("");
  textcolor(14); gotoxy(28,10);
  inkey();
  }


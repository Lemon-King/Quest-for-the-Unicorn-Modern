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

void combat_help() {
  textmode(3);
  textcolor(12); gotoxy(31,1);
  txt_writeln("Combat Commands:");
  txt_writeln("");
  textcolor(14);
  col = 0;
  row = 3;
  help_item("1 - Move down left",30,17,13);
  help_item("2 - Move down",30,17,13);
  help_item("3 - Move down right",30,17,13);
  help_item("4 - Move left",30,17,13);
  help_item("6 - Move right",30,17,13);
  help_item("7 - Move up left",30,17,13);
  help_item("8 - Move up",30,17,13);
  help_item("9 - Move up right",30,17,13);
  help_item("f - Fire weapon",30,17,13);
  help_item("g - Get item",30,17,13);
  help_item("h - Healing",30,17,13);
  help_item("H - Show icons",30,17,13);
  help_item("i - inventory",30,17,13);
  help_item("m - Cast magic spell",30,17,13);
  help_item("p - Party commands",30,17,13);
  help_item("P - Pick up and pack",30,17,13);
  help_item("R - Record sheet",30,17,13);
  help_item("T - Throw",30,17,13);
  help_item("u - Use item",30,17,13);
  help_item("v - View mode",30,17,13);
  help_item("{ - Save weapon set",30,17,13);
  help_item("[ - Load weapon set",30,17,13);
  help_item("} - Save gear set",30,17,13);
  help_item("] - Load gear set",30,17,13);
  help_item("? - List of commands",30,17,13);
  help_item("",30,17,13);
  help_item("Any other key acts as",30,17,13);
  help_item("rest 1 turn command",30,17,13);
  gotoxy(28,24); textcolor(10);
  txt_write("Press any key to continue");
  inkey();
  show_screen();
  }


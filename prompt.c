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

void prompt(char* display) {
  msg(display,10);
return;
  setfillstyle(1,0);
  bar(0,460,479,500);
  setcolor(14);
  settextstyle(0,0,1);
  outtextxy(1,470,display);
  }

void promptNS(char* display) {
  msg(display,-10);
  }

/* What ends a question: "Dir?" with Improved Interface, "Dir ?" without */
char* question_mark() {
  return (improved_interface == 'Y') ? "?" : " ?";
  }

void prompt_question(char* question,Int32 color) {
  msgf(color,"%s%s",question,question_mark());
  }

/* A question on the text grid where the cursor stands; returns the columns it took */
Int32 write_question(char* question) {
  txt_write(question);
  txt_write(question_mark());
  return (Int32)(strlen(question) + strlen(question_mark()));
  }


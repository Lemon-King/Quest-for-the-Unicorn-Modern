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

void show_headings() {
  char buffer[80];
  setcolor(0);
  settextstyle(0,0,1);
  outtextxy(148,frame_bottom-1,"                              ");
  outtextxy(148,frame_bottom-1,player.d_name);
  sprintf(buffer,"%d",player.diff);
  outtextxy(10,frame_bottom-1,buffer);
  setcolor(14);
  outtext_xy(TEXT_GRID_WIDTH - 24,1,"Depth  :");
  outtext_xy(TEXT_GRID_WIDTH - 24,2,"Date   :");
  outtext_xy(TEXT_GRID_WIDTH - 24,3,"HP     :");
  outtext_xy(TEXT_GRID_WIDTH - 24,4,"Mana   :");
  outtext_xy(TEXT_GRID_WIDTH - 24,5,"Level  :");
  }


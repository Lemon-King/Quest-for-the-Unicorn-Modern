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

/* Whether a square holding value can be passed: 0 when it blocks, -2 for a monster monstPass lets through, else -1 */
Int32 value_passable(Int32 value,char monstPass,char doorPass) {
  Int32 flag;
  flag = -1;
  if (value == 21) flag = 0;
  if (value == 22 && doorPass != 'Y') flag = 0;
  if (value == 30) flag = 0;
  if (value == 33 && doorPass != 'Y') flag = 0;
  if (value == 3) flag = 0;
  if (value == 4) flag = 0;
  if (value < 0 && monstPass != 'Y') flag = 0;
  if (value < 0 && monstPass == 'Y') flag = -2;
  if (value == 38) flag = 0;
  if (value == 39) flag = 0;
  if (value == 41) flag = 0;
  return flag;
  }

Int32 passable(Int32 x,Int32 y,char monstPass,char doorPass) {
  if (x<1 || x>99 || y<1 || y>99) return 0;
  return value_passable(player.map[x][y],monstPass,doorPass);
  }

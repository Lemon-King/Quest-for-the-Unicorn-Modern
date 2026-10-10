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

Int32 get_linked_msg(Int32 n) {
  Int32 i;
  i = 0;
  while (gen[i].region < 90) {
    if (gen[i].link == n) return i;
    i++;
    }
  return -1;
  }


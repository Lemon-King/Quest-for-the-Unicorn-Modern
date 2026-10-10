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

double item_cost(OBJECT_REC* item) {
  double ret;
  ret = item->cost;
  if (item->charges > 0) ret += (item->charges * item->costp);
  if (item->plus > 0) ret += (item->plus * item->costp);
  switch (item->num_sockets) {
    case 1: ret += 100; break;
    case 2: ret += 500; break;
    case 3: ret += 1000; break;
    }
  switch (item->num_enchantments) {
    case 1: ret += 100; break;
    case 2: ret += 500; break;
    case 3: ret += 1000; break;
    }
  if (ret < item->cost) ret = item->cost;
  return ret;
  }


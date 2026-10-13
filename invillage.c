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

/* Every village is the village map, 15. player.city keeps the last town after leaving it, so in_city decides. */
char in_village(void) {
  return (player.in_city == 'Y' && player.city == 15) ? 'Y' : 'N';
  }

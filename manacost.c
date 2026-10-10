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

Int32 mana_cost(Int32 mana) {
  double scale;
  Int32    cast;
  scale = get_skill(SK_MAGIC);
  scale /= (has_intrinsic(A_MANA)) ? 2.0 : 3.0;
  scale /= 100.0;
  if (scale > 0.80) scale = 0.80;
  cast = (Int32)(mana - (mana * scale));
/* trio of might */
/*
  if (player.found[11] != 0 && player.found[12] !=0 && player.found[13] != 0)
    cast /= 2;
*/
  if (cast < 1) cast = 1;
  return cast;
  }


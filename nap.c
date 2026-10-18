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

/* Hours typed in digits; ESC, with Improved Interface, naps not at all */
void nap() {
  TEXT_FIELD field;
  char       hours[5];

  strcpy(hours,"");
  open_log_field(&field,hours,sizeof(hours),"How long ? ",11);
  field.cancel = (improved_interface == 'Y');
  field.accepts = is_digit_key;
  if (!read_field(&field)) strcpy(hours,"");

  player.effect[PE_SLEEP] = mval(hours) * 60;
  if (player.effect[PE_SLEEP] > 0) napping = '*';
  if (mval(hours) > 0) stats.counts[STAT_HOURS_NAPPED] += mval(hours);
  clear_prompt();
  show_player();
  }

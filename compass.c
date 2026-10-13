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

Int32 compass(Int32 suggest) {
  Int32 dx,dy;
  Int32 px,py;

  /* Without the Compass change regions 4, 8 and 12 are placed at the start of the next row */
  if (change_compass == 'Y') {
    dx = getPX(links[suggest].region,links[suggest].x);
    dy = getPY(links[suggest].region,link_y(suggest));
    px = getPX(player.region,player.x);
    py = getPY(player.region,player.y);
    }
  else {
    dx = (links[suggest].region % 4) * 100 + links[suggest].x;
    dy = (links[suggest].region / 4) * 100 + link_y(suggest);
    px = (player.region % 4) * 100 + player.x;
    py = (player.region / 4) * 100 + player.y;
    }
  if (abs(px-dx)<4 && py<dy) return 92;
  if (abs(px-dx)<4 && py>dy) return 91;
  if (px<dx && abs(py-dy)<4) return 93;
  if (px>dx && abs(py-dy)<4) return 94;
  if (px<dx && py<dy) return 97;
  if (px>dx && py<dy) return 98;
  if (px<dx && py>dy) return 95;
  if (px>dx && py>dy) return 96;
  return 10;
  }


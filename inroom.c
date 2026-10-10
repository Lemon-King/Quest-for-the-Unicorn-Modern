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

Int32 cell_is_wall(Int32 x,Int32 y) {
  if (player.map[x][y] == 21) return -1;
  if (player.map[x][y] == 22) return -1;
  if (player.map[x][y] == 23) return -1;
  if (player.map[x][y] == 33) return -1;
  return 0;
  }

Int32 wall_in_block(Int32 sx,Int32 sy,Int32 ex,Int32 ey) {
  Int32 x,y;
  for (y=sy; y<=ey; y++)
    for (x=sx; x<=ex; x++)
      if (x > 0 && x < 100 && y > 0 && y < 100)
        if (cell_is_wall(x,y)) return -1;
  return 0;
  }

Int32 in_room(Int32 x,Int32 y) {
//  Int32 ix,iy;
  Int32 wallCount;
  if (player.gateway == '*') return 0;
  wallCount = 0;
/*
  for (ix=x-1; ix<=x+1; ix++)
    for (iy=y-1; iy<=y+1; iy++)
      if (cell_is_wall(ix,iy)) wallCount++;
  return (wallCount < 6) ? -1 : 0;
*/

  if (wall_in_block(x-1,y-1,x,y)) wallCount++;
  if (wall_in_block(x,y-1,x+1,y)) wallCount++;
  if (wall_in_block(x-1,y,x,y+1)) wallCount++;
  if (wall_in_block(x,y,x+1,y+1)) wallCount++;
  return (wallCount != 4) ? -1 : 0;
  }


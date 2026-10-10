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

typedef struct {
  Int32 x;
  Int32 y;
  Int32 count;
  } ELEMENT;

char find_path(Int32 startx,Int32 starty,Int32 endx,Int32 endy,char ignoreMonst) {
  ELEMENT path[1000];
  Int32     count;
  Int32 i;
  Int32 x,y;
  Int32 pos;
  char flag;
  if (los(startx,starty,endx,endy,'Y')) {
    if (startx >endx && starty  > endy && passable(startx-1,starty-1,'N','N')) return '7';
    if (startx==endx && starty  > endy && passable(startx  ,starty-1,'N','N')) return '8';
    if (startx <endx && starty  > endy && passable(startx+1,starty-1,'N','N')) return '9';
    if (startx >endx && starty == endy && passable(startx-1,starty  ,'N','N')) return '4';
    if (startx <endx && starty == endy && passable(startx+1,starty  ,'N','N')) return '6';
    if (startx >endx && starty  < endy && passable(startx-1,starty+1,'N','N')) return '1';
    if (startx==endx && starty  < endy && passable(startx  ,starty+1,'N','N')) return '2';
    if (startx <endx && starty  < endy && passable(startx+1,starty+1,'N','N')) return '3';
    }
  count = 1;
  pos = 0;
  path[0].x=endx;
  path[0].y = endy;
  path[0].count = 0;
  while (pos < count) {
    for (y=path[pos].y-1;y<=path[pos].y+1;y++)
      for (x=path[pos].x-1;x<=path[pos].x+1;x++)
        if (x == startx && y == starty) {
          if (path[pos].x  < startx && path[pos].y  > starty) return '1';
          if (path[pos].x == startx && path[pos].y  > starty) return '2';
          if (path[pos].x  > startx && path[pos].y  > starty) return '3';
          if (path[pos].x  < startx && path[pos].y == starty) return '4';
          if (path[pos].x  > startx && path[pos].y == starty) return '6';
          if (path[pos].x  < startx && path[pos].y  < starty) return '7';
          if (path[pos].x == startx && path[pos].y  < starty) return '8';
          if (path[pos].x  > startx && path[pos].y  < starty) return '9';
          }
        else if ((x != path[pos].x || y != path[pos].y) && passable(x,y,ignoreMonst,'Y')) {
          flag = '*';
          for (i=0; i<count; i++)
            if (path[i].x == x && path[i].y == y) {
//              if (path[i].count >= path[pos].count) {
//                path[i].x = -1;
//                path[i].y = -1;
//                }
              flag = ' ';
              }
          if (flag == '*') {
            path[count].x = x;
            path[count].y = y;
            path[count].count = path[pos].count + 1;
            count++;
            if (count == 1000) {
              return '5';
              }
            }
          }
      pos++;
    }
  return '5';
  }

/*
Int32 find_path(Int32 start_x,Int32 start_y,Int32 end_x,Int32 end_y,Int32 count) {
  Int32 directions[10];
  Int32 i;
  Int32 best_dir;
  Int32 best_count;
  Int32 xdir,ydir;
  Int32 x,y;

  x = start_x;
  y = start_y;
  xdir=0; ydir=0;
  if (x < end_x && y < end_y) { xdir = 1; ydir = 1; }
  if (x > end_x && y < end_y) { xdir = -1; ydir = 1; }
  if (x < end_x && y > end_y) { xdir = 1; ydir = -1; }
  if (x > end_x && y > end_y) { xdir = -1; ydir = -1; }
  if (x == end_x && y < end_y) ydir = 1;
  if (x == end_x && y > end_y) ydir = -1;
  if (x < end_x && y == end_y) xdir = 1;
  if (x > end_x && y == end_y) xdir = -1;
  if (passable(start_x+xdir,start_y+ydir,'N')) { x+=xdir; y+=ydir; }
  else if (passable(start_x+xdir,start_y,'N')) x+=xdir;
  else if (passable(start_x,start_y+ydir,'N')) y+=ydir;
  else if (xdir == 0 && passable(start_x+1,start_y+ydir,'N')) { x++; y+=ydir; }
  else if (xdir == 0 && passable(start_x-1,start_y+ydir,'N')) { x--; y+=ydir; }
  else if (ydir == 0 && passable(start_x+xdir,start_y-1,'N')) { x+=xdir; y--; }
  else if (ydir == 0 && passable(start_x+xdir,start_y+1,'N')) { x+=xdir; y++; }
  if (x<start_x && y>start_y) return '1';
  if (x==start_x && y>start_y) return '2';
  if (x>start_x && y>start_y) return '3';
  if (x<start_x && y==start_y) return '4';
  if (x>start_x && y==start_y) return '6';
  if (x<start_x && y<start_y) return '7';
  if (x==start_x && y<start_y) return '8';
  if (x>start_x && y<start_y) return '9';
  return '5';





  if (count == 0) best_path = PATH_MAX;
  if (start_x == end_x && start_y == end_y) {
    best_path = count;
    return 0;
    }
  for (i=0; i<count; i++)
    if (path_x[i] == start_x && path_y[i] == start_y) return -1;
  count++;
  if (count > best_path) return -1;
  if (count >= PATH_MAX) return -1;
  path_x[count-1] = start_x;
  path_y[count-1] = start_y;
  for (i=1; i<=9; i++) directions[i] = -2;
  for (i=1; i<=9; i++)
    if (i != 5) {
      switch(i) {
        case 1: xdir = -1; ydir = +1; break;
        case 2: ydir = +1; break;
        case 3: xdir = +1; ydir = +1; break;
        case 4: xdir = -1; break;
        case 6: xdir = +1; break;
        case 7: xdir = -1; ydir = -1; break;
        case 8: ydir = -1; break;
        case 9: xdir = +1; ydir = -1; break;
        }
      if (passable(start_x + xdir,start_y + ydir,'N') == 0) 
        directions[i] = -1;
      else
        directions[i] = find_path(start_x + xdir,start_y + ydir,end_x,end_y,count);
      }
  best_count = PATH_MAX;
  best_dir = 5;
  for (i=2; i<=9; i+=2) {
    if (directions[i] >= 0 && directions[i] < best_count) {
      best_dir = i;
      best_count = directions[i];
      }
    }
  for (i=1; i<=9; i+=2) {
    if (i != 5 ) {
      if (directions[i] >= 0 && directions[i] < best_count) {
        best_dir = i;
        best_count = directions[i];
        }
      }
    }
  if (count == 1) return best_dir + '0';
  if (best_dir == 5) return -1;
  return best_count+1;
  }

*/

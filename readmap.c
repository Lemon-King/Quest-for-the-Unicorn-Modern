/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 * One map file: 100 lines of exactly 100 characters, one tile per character. Lines end in LF or CRLF, and DOS
 * editors ended the file with Ctrl+Z (0x1A). Row 0 and column 0 are not in the file and hold plains. The map is
 * stored [y][x]; copyMap() turns it into player.map's [x][y].
 */

#include "header.h"

/* The tile each character stands for; any other character is stone floor */
static Int32 map_tile(char c) {
  if (c >= 'B' && c <= 'Q') return 520 + (c - 'B');   /* Merchants */
  switch (c) {
    case ' ':return 1;                          /* Plains                */
    case '.':return 1;                          /* Plains                */
    case '#':return 2;                          /* Forest                */
    case '^':return 3;                          /* Mountains             */
    case 'v':return 177;                        /* Mountain Floor        */
    case '~':return 4;                          /* Water                 */
    case '-':return 8;                          /* Desert                */
    case '=':return 6;                          /* Swamp                 */
    case '(':return 9;                          /* Hills                 */
    case '!':return 17;                         /* Village               */
    case '@':return 21;                         /* Brick Wall            */
    case '+':return 7;                          /* Stone Floor           */
    case '\\':return 23;                        /* Open Door             */
    case '/':return 22;                         /* Closed Door           */
    case '|':return 14;                         /* Dock                  */
    case '%':return 20;                         /* Tree                  */
    case 's':return 13;                         /* Ship                  */
    case ',':return 15;                         /* Wood Floor            */
    case 'c':return 26;                         /* Chair                 */
    case 't':return 27;                         /* Table                 */
    case 'b':return 29;                         /* Bed                   */
    case 'h':return 28;                         /* Merchant              */
    case '}':return 32;                         /* Gateway               */
    case 'm':return 30;                         /* Shelves               */
    case '1':return 5;                          /* City                  */
    case 'A':return 11;                         /* Dungeon Entrance      */
    case 'Z':return 536;                        /* Gacha Machine         */
    default :return 7;                          /* Stone Floor           */
    }
  }

/* Fills map from filename and returns 1, or returns 0 with error naming the file and the line at fault */
Int32 read_map(char* filename,MAP_TYPE* map,char* error,Int32 error_size) {
  FILE* file;
  char  line[1024];
  Int32 x,y;

  file = fopen(filename,"rb");
  if (file == NULL) {
    snprintf(error,error_size,"could not open %s",filename);
    return 0;
    }

  for (x=0; x<=100; x++) (*map)[0][x] = 1;
  for (y=1; y<=100; y++) {
    if (fgets(line,sizeof(line),file) == NULL) {
      snprintf(error,error_size,"%s has %d lines, expected 100",filename,y-1);
      fclose(file);
      return 0;
      }
    if (strcspn(line,"\r\n\x1a") != 100) {
      snprintf(error,error_size,"%s, line %d is not 100 characters long",filename,y);
      fclose(file);
      return 0;
      }
    (*map)[y][0] = 1;
    for (x=1; x<=100; x++) (*map)[y][x] = map_tile(line[x-1]);
    }

  /* Only empty lines may follow the last row */
  while (fgets(line,sizeof(line),file) != NULL) {
    if (strspn(line,"\r\n\x1a") != strlen(line)) {
      snprintf(error,error_size,"%s has more than 100 lines",filename);
      fclose(file);
      return 0;
      }
    }

  fclose(file);
  return 1;
  }

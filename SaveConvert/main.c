/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#define MAIN

#include "header.h"

extern char fileversion[64];

int main(int argc,char** argv) {
  int i;
  if (argc != 2) {
    printf("Usage: saveconvert filename\n");
    exit(1);
    }
  strcpy(loadname, argv[1]);
  printf("Loading %s\n",loadname);
  for (i=0; i<strlen(loadname); i++)
    if (loadname[i] == '.') loadname[i] = 0;
  load();
printf("File version: %s\n",fileversion);
  save_new();
  return 0;
  }

